using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using Windows.Storage;

namespace Humidor
{
    public class MySettings
    {
        
        private Windows.Storage.ApplicationDataContainer appData
        {
            get
            {
                return  Windows.Storage.ApplicationData.Current.LocalSettings;
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
                if(appData.Values[key] != null)  return appData.Values[key].ToString();
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
                /*int id; we will alwys be working with it as a string so no point in converting..
                string sid = GetSetting("uid");
	            bool res = int.TryParse(sid, out id);
                if (res == false)  id = 0;
                return id;*/
            }
            set
            {
                SaveSettings("uid", value.ToString() );
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

        public DateTime? LastUpdate
        {
            get
            {
                try
                {
                    DateTime dt = Convert.ToDateTime(GetSetting("LastUpdate"));
                    return dt;
                }
                catch (Exception ex)
                {
                    return null;
                }
            }
            set
            {
                SaveSettings("LastUpdate", value.ToString());
            }
        }
         
    }
}
