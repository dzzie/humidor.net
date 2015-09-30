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
using Windows.Security.ExchangeActiveSyncProvisioning;
using Windows.UI.Notifications;
using Windows.Data.Xml.Dom;
using Windows.ApplicationModel.Background;
using System.Threading.Tasks;
using Windows.ApplicationModel;
/*notes: 
 * in general live tile will update every 30 min from background task, but on first start of it we autoupdate it immediatly from within this code.
   if emulator is detected default settings are used and timer speedup for refresh
 */
 
namespace Humidor
{
    
    public static class DeviceInfo
    {
        // igrali.com/2014/07/17/get-device-information-windows-phone-8-1-winrt/
        private static EasClientDeviceInformation deviceInfo = new EasClientDeviceInformation();
        
        public static bool IsRunningOnEmulator
        {
            get
            {
                return (deviceInfo.SystemProductName == "Virtual");
            }
        }
    }

    public sealed partial class PivotPage : Page
    {
        DispatcherTimer timer = new DispatcherTimer();
        private bool liveTileEnabled = false;
        private bool justEnabledTile = false;
        private string content;
        private bool _debug = true;

        private string strNavFailed = @"<html><body bgcolor=black><br>
				       			        <font style='font-family: Segoe WP; font-size:80px; color: gray'>
                                        Server not Reachable
                                        </body></html>";

        public PivotPage()
        {
            this.InitializeComponent();

            if (DeviceInfo.IsRunningOnEmulator) 
            {
                App.settings.uid = "6";
                App.settings.apiKey = "test"; 
                App.settings.server = "192.168.0.10";
            }

            txtUserID.Text = App.settings.uid;
            txtApiKey.Text = App.settings.apiKey;
            txtServer.Text = App.settings.server;

            setLiveTile(App.settings.liveTile == "true" ? true : false);

            var myPackage = Windows.ApplicationModel.Package.Current;
            PackageVersion ver = myPackage.Id.Version;
            txtVersion.Text = "Version: " + ver.Major.ToString() + "." + ver.Minor.ToString() + "." + ver.Build.ToString() + "." + ver.Revision.ToString();

            //they havent saved a config yet..so lets jump to it
            if (this.pivot.SelectedIndex == 0 && App.settings.uid == "") {
                this.pivot.SelectedIndex = 2;
                return;
            }

            ShowStats();

        }

        private void setLiveTile(bool enabled)
        {
            //if (liveTileEnabled && enabled) return;
            //if (!liveTileEnabled && !enabled) return;
            if (!liveTileEnabled && enabled) justEnabledTile = true;
            liveTileEnabled = enabled;
            App.settings.liveTile = liveTileEnabled ? "true" : "";
            btnLiveTile.Content = liveTileEnabled ? "disable" : "enable";
            RegisterTask(liveTileEnabled);
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
                if (!_debug) return;
                x.LoadXml(getXML("Error parsing XML"));
            }

            var tileNotification = new TileNotification(x);
            TileUpdateManager.CreateTileUpdaterForApplication().Update(tileNotification);
        }

        private async void RegisterTask(bool enable)
        {

            string myTaskName = "FirstTask";

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
                    return;
                }
            }

            if (!enable) return;
            
            //NotificationSetting ns = TileUpdateManager.CreateTileUpdaterForApplication().Setting;
            //if (ns != NotificationSetting.Enabled) return; //not pinned or not enabled so dont register..
            
            if (justEnabledTile)
            {
                UpdateTile();
                justEnabledTile = false;
            }
           
            await BackgroundExecutionManager.RequestAccessAsync();
            BackgroundTaskBuilder taskBuilder = new BackgroundTaskBuilder { Name = myTaskName, TaskEntryPoint = "MyTask.FirstTask" };
            taskBuilder.SetTrigger(new TimeTrigger(30,false));
            BackgroundTaskRegistration myFirstTask = taskBuilder.Register();
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
            if(_debug) btnSave.Content = String.Format("{0:M.d h:mm}", App.settings.LastUpdate);
        }

        void wb_NavigationFailed(object sender,  WebViewNavigationFailedEventArgs e){
            wb.NavigateToString(strNavFailed);
        }

        void wb2_NavigationFailed(object sender, WebViewNavigationFailedEventArgs e)
        {
            wb2.NavigateToString(strNavFailed);
        }

        private void ShowStats()
        {
            
            string isDark = Application.Current.RequestedTheme == ApplicationTheme.Dark ? "1" : "0"; 

            //any why in the world doesnt Navigate accept a string? oghh yeah academics..
            string url = "http://" + txtServer.Text + "/humidor/mobile.php?id=" + txtUserID.Text + "&isDark=" + isDark;           
            wb.Navigate(new Uri(url));
            wb2.Navigate(new Uri(url + "&page=2"));
            App.settings.LastUpdate = DateTime.Now;
            if (!timer.IsEnabled) Start_timer();
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
                showMessage("Server Response", ret, false);
            }
            catch (Exception ex) {
                showMessage("Exception Caught", ret, false);
            } 
          
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
                if (_debug)
                {
                    //if (this.content == null) this.content = ex.ToString();
                    return;
                }
                this.content = null;
            }

        }
        public async void showMessage(string title, string msg, bool fullScreen)
        {
            //MessageDialog message = new MessageDialog(msg);
            //await message.ShowAsync();
            ContentDialog contentDialog = new ContentDialog();
            contentDialog.FullSizeDesired =fullScreen;
            contentDialog.Title = title;
            contentDialog.Content = msg;
            contentDialog.PrimaryButtonText = "OK";
            await contentDialog.ShowAsync();
        }

        private void btnSave_Click(object sender, RoutedEventArgs e)
        {
            App.settings.uid = txtUserID.Text;
            App.settings.apiKey = txtApiKey.Text;
            App.settings.server = txtServer.Text;
            ShowStats();
            this.pivot.SelectedIndex = 0;
        }

        private void btnSmoked_Click(object sender, RoutedEventArgs e)
        {
            string url = "http://" + txtServer.Text + "/humidor/logData.php?wasSmoked=1&clientid=" + txtUserID.Text + "&apikey=" + App.settings.apiKey;
            doWebReq(url);
        }

        private void btnWatered_Click(object sender, RoutedEventArgs e)
        {
            string url = "http://" + txtServer.Text + "/humidor/logData.php?wasWatered=1&clientid=" + txtUserID.Text + "&apikey=" + App.settings.apiKey;
            doWebReq(url);
        }

        private void btnClearAlert_Click(object sender, RoutedEventArgs e)
        {
            string url = "http://" + txtServer.Text + "/humidor/logData.phpclear_alert=1&clientid=" + txtUserID.Text + "&apikey=" + App.settings.apiKey;
            doWebReq(url);
        }

        private void btnLiveTile_Click(object sender, RoutedEventArgs e)
        {
            setLiveTile(!liveTileEnabled);
        }

        private void Current_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            string CurrentViewState = ApplicationView.GetForCurrentView().Orientation.ToString();
            bool isPortrait = (CurrentViewState == "Portrait"); //VERTICAL

            /*if (isPortrait) { 
                wb.Width = Window.Current.Bounds.Width;
                wb.Height = Window.Current.Bounds.Height;
                wb2.Width = Window.Current.Bounds.Width;
                wb2.Height = Window.Current.Bounds.Height;
            }
            else
            {
                wb.Width = Window.Current.Bounds.Height;
                wb.Height = Window.Current.Bounds.Width;
                wb2.Width = Window.Current.Bounds.Height;
                wb2.Height = Window.Current.Bounds.Width;
            }*/

            double newTop = isPortrait ? 354 : 34;
            btnSave.Margin = new Thickness(btnSave.Margin.Left, newTop, btnSave.Margin.Right, btnSave.Margin.Bottom);
            //btnSave.Margin.Top = isPortrait ? 254 : 354; //because this would be to easy..

           
        }

        async private void btnAbout_Click(object sender, RoutedEventArgs e)
        {
            string url = "http://sandsprite.com/blogs/index.php?uid=10&pid=315";
            await Windows.System.Launcher.LaunchUriAsync(new Uri(url));
        }
       
    }
}
