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

namespace Humidor
{
    
    public static class DeviceInfo
    {
        //http://igrali.com/2014/07/17/get-device-information-windows-phone-8-1-winrt/
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

            //they havent saved a config yet..so lets jump to it
            if (this.pivot.SelectedIndex == 0 && App.settings.uid == "") {
                this.pivot.SelectedIndex = 2;
                return;
            }

            ShowStats();

        }

        private void ShowStats()
        {
            if (this.pivot.SelectedIndex == 2) this.pivot.SelectedIndex = 0; //they just saved settings so lets jump to the display..

            //any why in the world doesnt Navigate accept a string? oghh yeah academics..
            string url = "http://" + txtServer.Text + "/humidor/mobile.php?id=" + txtUserID.Text;           
            wb.Navigate(new Uri(url));
            wb2.Navigate(new Uri(url + "&page=2"));
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

        //save button
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            App.settings.uid = txtUserID.Text;
            App.settings.apiKey = txtApiKey.Text;
            App.settings.server = txtServer.Text;
            ShowStats();
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

        private void Current_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            string CurrentViewState = ApplicationView.GetForCurrentView().Orientation.ToString();
            //StatusTxtBlck.Text = "Curren Page Orientation is: " + CurrentViewState;
            bool isPortrait = (CurrentViewState == "Portrait"); //VERTICAL

            if (isPortrait) { 
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
            }

            //btnSave.VerticalAlignment = VerticalAlignment.Center;
            double newTop = isPortrait ? 354 : 34;
            btnSave.Margin = new Thickness(btnSave.Margin.Left, newTop, btnSave.Margin.Right, btnSave.Margin.Bottom);
            //btnSave.Margin.Top = isPortrait ? 254 : 354; //because this would be to easy..

           
        }
       
    }
}
