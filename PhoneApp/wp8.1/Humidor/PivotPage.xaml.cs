using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Globalization;
using System.IO;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.ApplicationModel.Resources;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Graphics.Display;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Popups;
using Windows.UI.ViewManagement;
using Windows.Web.Http;
using Windows.UI.Notifications;
using Windows.Data.Xml.Dom;
using Windows.ApplicationModel.Background;
using System.Threading.Tasks;
using Windows.ApplicationModel;
using Humidor.Common;
 
namespace Humidor
{

    public sealed partial class PivotPage : Page
    {
        private NavigationHelper navigationHelper;
        private ObservableDictionary defaultViewModel = new ObservableDictionary();

        DispatcherTimer timer = new DispatcherTimer();

        //todo: refresh button for this? or meta refresh or js?
        private string strNavFailed = @"<html><body bgcolor=XXXXX><br>
				       			        <font style='font-family: Segoe WP; font-size:80px; color: ZZZZZ'>
                                        Server not Reachable
                                        </body></html>";

        public PivotPage()
        {
            this.InitializeComponent();

            this.navigationHelper = new NavigationHelper(this);
            //this.navigationHelper.LoadState += this.NavigationHelper_LoadState;
            //this.navigationHelper.SaveState += this.NavigationHelper_SaveState;

        }

        public void Start_timer()
        {
            timer.Tick += timer_Tick;

            if (DeviceInfo.IsRunningOnEmulator)
                timer.Interval = new TimeSpan(00, 0, 10);
            else 
                timer.Interval = new TimeSpan(00, 30, 00);

            bool enabled = timer.IsEnabled;
            timer.Start();
        }

        //refresh our webviews on interval
        void timer_Tick(object sender, object e)
        {
            ShowStats();
            //if(_debug) btnSave.Content = String.Format("{0:M.d h:mm}", App.settings.LastUpdate);
        }

        void wb_NavigationFailed(object sender,  WebViewNavigationFailedEventArgs e){
            bool isDark = (Application.Current.RequestedTheme == ApplicationTheme.Dark);
            string bgColor = isDark ? "black" : "white";
            string fontColor = isDark ? "gray" : "black";
            string html = strNavFailed.Replace("XXXXX", bgColor).Replace("ZZZZZ", fontColor);
            wb.NavigateToString(html);
        }

        void wb2_NavigationFailed(object sender, WebViewNavigationFailedEventArgs e)
        {
            bool isDark = (Application.Current.RequestedTheme == ApplicationTheme.Dark);
            string bgColor = isDark ? "black" : "white";
            string fontColor = isDark ? "gray" : "black";
            string html = strNavFailed.Replace("XXXXX", bgColor).Replace("ZZZZZ", fontColor);
            wb2.NavigateToString(html);
        }

        private void ShowStats()
        {
            
            string isDark = Application.Current.RequestedTheme == ApplicationTheme.Dark ? "1" : "0"; 

            //any why in the world doesnt Navigate accept a string? oghh yeah academics..
            string url = "http://" + App.settings.server + "/humidor/mobile.php?id=" + App.settings.uid + "&isDark=" + isDark;
            try
            {
                wb.Navigate(new Uri(url));
                wb2.Navigate(new Uri(url + "&page=2"));
                App.settings.LastUpdate = DateTime.Now;
                if (!timer.IsEnabled) Start_timer();
            }
            catch (Exception)
            {
                wb_NavigationFailed(null, null);
                wb2_NavigationFailed(null, null);
            }
        }

        async private void doWebReq(string url)
        {
            
            string ret = "";
            try
            {
                var httpClient = new HttpClient();
                var response = await httpClient.GetAsync(new Uri(url, UriKind.RelativeOrAbsolute));
                ret = response.StatusCode.ToString();
                response.EnsureSuccessStatusCode(); //If Response is not Http 200 then EnsureSuccessStatusCode will throw an exception
                ret = await response.Content.ReadAsStringAsync();
                App.showMessage("Server Response", ret, false);
            }
            catch (Exception ex) {
                App.showMessage("Exception Caught", ret, false);
            } 
          
        }

        private void btnSmoked_Click(object sender, RoutedEventArgs e)
        {
            string url = "http://" + App.settings.server + "/humidor/logData.php?wasSmoked=1&clientid=" + App.settings.uid + "&apikey=" + App.settings.apiKey;
            doWebReq(url);
        }

        private void btnWatered_Click(object sender, RoutedEventArgs e)
        {
            string url = "http://" + App.settings.server + "/humidor/logData.php?wasWatered=1&clientid=" + App.settings.uid  + "&apikey=" + App.settings.apiKey;
            doWebReq(url);
        }

        private void btnClearAlert_Click(object sender, RoutedEventArgs e)
        {
            string url = "http://" + App.settings.server + "/humidor/logData.php?clear_alert=1&clientid=" + App.settings.uid + "&apikey=" + App.settings.apiKey;
            doWebReq(url);
        }

        private void btnAboutPage_Click(object sender, RoutedEventArgs e)
        {
            this.Frame.Navigate(typeof(About));
        }

        private void btnConfigPage_Click(object sender, RoutedEventArgs e)
        {
            this.Frame.Navigate(typeof(Config));
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            this.navigationHelper.OnNavigatedTo(e);
            ShowStats();
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
    }
}
