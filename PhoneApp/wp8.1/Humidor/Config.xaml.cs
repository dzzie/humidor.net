using Humidor.Common;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Graphics.Display;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.Data.Xml.Dom;
using Windows.ApplicationModel.Background;
using System.Threading.Tasks;
using Windows.ApplicationModel;
using Windows.UI.Notifications;
using Windows.Web.Http;

namespace Humidor
{

    public sealed partial class Config : Page
    {
        private NavigationHelper navigationHelper;
        private ObservableDictionary defaultViewModel = new ObservableDictionary();
        private bool liveTileEnabled = false;
        private string content;

        public Config()
        {
            this.InitializeComponent();

            this.navigationHelper = new NavigationHelper(this);
            this.navigationHelper.LoadState += this.NavigationHelper_LoadState;
            this.navigationHelper.SaveState += this.NavigationHelper_SaveState;

        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            this.navigationHelper.OnNavigatedTo(e);

            if (App.debug && DeviceInfo.IsRunningOnEmulator)
            {
                App.settings.uid = "6";
                App.settings.apiKey = "test";
                App.settings.server = "192.168.0.10";
            }

            txtUserID.Text = App.settings.uid;
            txtApiKey.Text = App.settings.apiKey;
            txtServer.Text = App.settings.server;
            liveTileEnabled = App.settings.liveTile == "true" ? true : false;
            btnLiveTile.Content = liveTileEnabled ? "disable" : "enable";
        }

        private void btnLiveTile_Click(object sender, RoutedEventArgs e)
        {
            Task<bool> outerTask = RegisterTask(!liveTileEnabled);
            /*outerTask.ContinueWith(task =>
            {
                if (task.Result)
                {
                    //we can not update button text here wrong thread error..overly complex bs
                }
            });*/
        }

        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            this.Frame.Navigate(typeof(PivotPage));
        }

        private void btnSave_Click(object sender, RoutedEventArgs e)
        {
            App.settings.uid = txtUserID.Text;
            App.settings.apiKey = txtApiKey.Text;
            App.settings.server = txtServer.Text;
            this.Frame.Navigate(typeof(PivotPage));
        }

        private async Task<bool> RegisterTask(bool enable)
        {

            string myTaskName = "FirstTask";
            bool hadError = false;

            bool justEnabledTile = false;
            if (!liveTileEnabled && enable) justEnabledTile = true;

            if (!enable)
            {
                //reset live tile back to original icon
                TileUpdater updater = TileUpdateManager.CreateTileUpdaterForApplication();
                updater.Clear();
            }

            foreach (var cur in BackgroundTaskRegistration.AllTasks)
            {
                if (cur.Value.Name == myTaskName)
                {
                    if (!enable) cur.Value.Unregister(true);
                    goto retNow;
                }
            }

            if (!enable) goto retNow; 
            
            if (await BackgroundExecutionManager.RequestAccessAsync() == BackgroundAccessStatus.Denied)
            {
                App.showMessage("Register Task Failed",
                    "Access to register a background task failed.\n\n" +
                    "You can enable access for this app in:\n" +
                    "   settings -> battery -> usage\n\n" +
                    "This is required for the live tile to\n" +
                    "update every 30 minutes.", false);
                hadError = true;
                goto retNow;
            }

            NotificationSetting ns = TileUpdateManager.CreateTileUpdaterForApplication().Setting;
            if (ns != NotificationSetting.Enabled)
            {
                //note this is not a detection if tile was pinned or not..
                App.showMessage("Tile not updatable", "Tile updating has been disabled?", false);
                hadError = true;
                goto retNow;
            }

            if (justEnabledTile)
            {
                UpdateTile();
                justEnabledTile = false;
            }

            BackgroundTaskBuilder taskBuilder = new BackgroundTaskBuilder { Name = myTaskName, TaskEntryPoint = "MyTask.FirstTask" };
            taskBuilder.SetTrigger(new TimeTrigger(30, false));
            BackgroundTaskRegistration myFirstTask = taskBuilder.Register();
        
     retNow:
            //we can not update the button from Task.onComplete because wrong thread error..so this is forced..
            if (!hadError)
            {
                liveTileEnabled = enable;
                App.settings.liveTile = liveTileEnabled ? "true" : "";
                btnLiveTile.Content = liveTileEnabled ? "disable" : "enable";
            }
            return !hadError;

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

        async private void UpdateTile()
        {
            string server = App.settings.server;
            string uid = App.settings.uid;

            if (server.Length == 0 || uid.Length == 0) return;

            string url = "http://" + server + "/humidor/mobile.php?id=" + uid + "&page=3";

            await webReq2str(url);

            if (content == null || content.Length == 0) return;

            string xml = getXML(content);
            XmlDocument x = new XmlDocument();
            try
            {
                x.LoadXml(xml);
            }
            catch (Exception ex)
            {
                if (!App.debug) return;
                x.LoadXml(getXML("Error parsing XML"));
            }

            var tileNotification = new TileNotification(x);
            TileUpdateManager.CreateTileUpdaterForApplication().Update(tileNotification);
        }


        async private Task webReq2str(string url)
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
                if (App.debug)
                {
                    //if (this.content == null) this.content = ex.ToString();
                    return;
                }
                this.content = null;
            }

        }

        private void Current_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            /*
            string CurrentViewState = ApplicationView.GetForCurrentView().Orientation.ToString();
            bool isPortrait = (CurrentViewState == "Portrait"); //VERTICAL

            double newTop = isPortrait ? 354 : 34;
            btnSave.Margin = new Thickness(btnSave.Margin.Left, newTop, btnSave.Margin.Right, btnSave.Margin.Bottom);
            //btnSave.Margin.Top = isPortrait ? 254 : 354; //because this would be to easy..
            */
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            this.navigationHelper.OnNavigatedFrom(e);
        }


        public NavigationHelper NavigationHelper
        {
            get { return this.navigationHelper; }
        }

        public ObservableDictionary DefaultViewModel
        {
            get { return this.defaultViewModel; }
        }

        private void NavigationHelper_LoadState(object sender, LoadStateEventArgs e)
        {
        }

        private void NavigationHelper_SaveState(object sender, SaveStateEventArgs e)
        {
        }
       
    }
}
