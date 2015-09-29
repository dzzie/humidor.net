using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.ApplicationModel.Background;
using Windows.Web.Http;
using Windows.Data.Xml.Dom;
using Windows.UI.Notifications;

namespace MyTask
{
    public sealed class FirstTask : IBackgroundTask
    {
        private MySettings settings = new MySettings();
        private bool _debug = true;
        private string content;

        async private Task doWebReq(string url) 
        {
            content = null;
            try
            {
                var httpClient = new HttpClient();
                var response = await httpClient.GetAsync(new Uri(url, UriKind.RelativeOrAbsolute));
                this.content = response.StatusCode.ToString();
                response.EnsureSuccessStatusCode(); //If Response is not Http 200 then EnsureSuccessStatusCode will throw an exception
                this.content = await response.Content.ReadAsStringAsync();
            }
            catch (Exception ex)
            {
                if (_debug) {
                    if (this.content == null) this.content = "Exception in webReq";
                    return;
                }
                this.content = null;
            }

        }

        private string getXML(string msg)
        {

            if (msg == null) msg = "[null]";
            if (msg.Length == 0) msg = "[Empty]";

            string xml = String.Format(@"<tile>
                                          <visual>
                                            <binding template='TileSquareText04'>
                                              <text id='1'>{0}</text>
                                            </binding>  
                                          </visual>
                                        </tile>", msg);

            return xml;
        }

        async private Task UpdateTile()
        {
            string server = settings.server;
            string uid = settings.uid;

            if (server.Length == 0 || uid.Length == 0) return;

            string url = "http://" + server + "/humidor/mobile.php?id=" + uid + "&page=3";

            await doWebReq(url);

            if (content == null || content.Length == 0) return;

            string xml = getXML(content);

            XmlDocument x = new XmlDocument();
            try
            {
                x.LoadXml(xml);
            }
            catch (Exception ex)
            {
                if (!_debug) return;
                x.LoadXml(getXML("Error Parsing WebXml"));
            }

            var tileNotification = new TileNotification(x);
            TileUpdateManager.CreateTileUpdaterForApplication().Update(tileNotification);
        }

        async public void Run(IBackgroundTaskInstance taskInstance)
        {
            BackgroundTaskDeferral deferral = taskInstance.GetDeferral();
            await UpdateTile();
            deferral.Complete();
        }
    }

    class MySettings
    {


        private Windows.Storage.ApplicationDataContainer appData
        {
            get
            {
                return Windows.Storage.ApplicationData.Current.LocalSettings;
            }
        }

        private bool SaveSettings(string key, string value)
        {
            try
            {
                appData.Values[key] = value;
                return true;
            }
            catch
            {
                return false;
            }
        }

        private string GetSetting(string key)
        {
            try
            {
                if (appData.Values[key] != null) return appData.Values[key].ToString();
                return "";
            }
            catch
            {
                return "";
            }
        }

        public string uid
        {
            get
            {
                return GetSetting("uid");
            }
            set
            {
                SaveSettings("uid", value.ToString());
            }
        }

        public string apiKey
        {
            get
            {
                return GetSetting("apiKey");
            }
            set
            {
                SaveSettings("apiKey", value);
            }
        }

        public string server
        {
            get
            {
                return GetSetting("server");
            }
            set
            {
                SaveSettings("server", value);
            }
        }
    }

}
