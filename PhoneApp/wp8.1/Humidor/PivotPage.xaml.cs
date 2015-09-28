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
using Windows.UI.ViewManagement;

// The Pivot Application template is documented at http://go.microsoft.com/fwlink/?LinkID=391641

namespace Humidor
{
    public sealed partial class PivotPage : Page
    {

        public PivotPage()
        {
            this.InitializeComponent();

            if (false)
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
            //yes yes i know should be https..this is not critical data, you can leave apikey out if you want only used for watered/smoked buttons
            string url = "http://" + txtServer.Text + "/humidor/mobile.php?id=" + txtUserID.Text + "&apikey=" + txtApiKey.Text; // +"&page=" + this.pivot.SelectedIndex.ToString();            
            wb.Navigate(new Uri(url));
            wb2.Navigate(new Uri(url + "&page=2"));
        }

        //save button
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            App.settings.uid = txtUserID.Text;
            App.settings.apiKey = txtApiKey.Text;
            App.settings.server = txtServer.Text;
            ShowStats();
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
