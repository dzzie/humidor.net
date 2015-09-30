using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Humidor.Common;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkID=390556

namespace Humidor
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class About : Page
    {

        private NavigationHelper navigationHelper;
        private ObservableDictionary defaultViewModel = new ObservableDictionary();

        public About()
        {
            this.InitializeComponent();

            this.navigationHelper = new NavigationHelper(this);
            //this.navigationHelper.LoadState += this.NavigationHelper_LoadState;
            //this.navigationHelper.SaveState += this.NavigationHelper_SaveState;

            var myPackage = Windows.ApplicationModel.Package.Current;
            Windows.ApplicationModel.PackageVersion ver = myPackage.Id.Version;
            txtVersion.Text = "Version: " + ver.Major.ToString() + "." + ver.Minor.ToString() + "." + ver.Build.ToString() + "." + ver.Revision.ToString();
 

        }

        public NavigationHelper NavigationHelper
        {
            get { return this.navigationHelper; }
        }

        public ObservableDictionary DefaultViewModel
        {
            get { return this.defaultViewModel; }
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            this.navigationHelper.OnNavigatedTo(e);
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            this.navigationHelper.OnNavigatedFrom(e);
        }

        async private void btnAbout_Click(object sender, RoutedEventArgs e)
        {
            string url = "http://sandsprite.com/blogs/index.php?uid=10&pid=315";
            await Windows.System.Launcher.LaunchUriAsync(new Uri(url));
        }


    }
}
