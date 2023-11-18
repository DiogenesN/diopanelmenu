# DioPanelMenu
A simple nice fast application menu.
You can use it independently or together with DioAppFinder.
With DioAppFinder you can add your favorite applications to the menu.
If you use it independently then add the full path to your .desktop file to: .config/diopanel/diomenu/diomenupaths.conf.
It was tested on Debian 12.

# What you can do with DioPanelMenu
   1. Quickly access/launch your favorite applications.
   2. Adjust width, height, position and opacity of the panel.
   3. Adjust height and position of the menu.
   4. Add your custom commands for system settings, reboot and poweroff.

# Note
   DioAppFinder is recommended to easily add your favorite applications:
   https://github.com/DiogenesN/dioappfinder

# Installation/Usage
  1. Open a terminal and run:

		 ./configure

  2. if all went well then run:

		 make
		 sudo make install
		 
		 (if you just want to test it then run: make run)
		
  4. Run the application or add it to startup:
  
		 diopanelmenu

 After you lauch the application you will see a menu button:
 
![Alt text](https://github.com/DiogenesN/dioapanelmenu/blob/main/panelbutton.png)

 If you right click on it, you will get the panel settings, adjust the desired size and position:
 
![Alt text](https://github.com/DiogenesN/dioapanelmenu/blob/main/panelsettings.png)

 If you left click on it, you will get an empty menu, to add your favorite applications to it use DioAppFinder (or add full path to config).
 Here is what the menu looks like with applications added:

![Alt text](https://github.com/DiogenesN/dioapanelmenu/blob/main/panelmenu.png)

On the top there is the user icon, user name, menu settings button, custom: system settings, reboot and poweroff buttons.
Click on the menu settings to adjust size, position and custom actions:

![Alt text](https://github.com/DiogenesN/dioapanelmenu/blob/main/panelmenusettings.png)
![Alt text](https://github.com/DiogenesN/dioapanelmenu/blob/main/panelmenusettingsdialog.png)

That's it!

 Make sure you have the following packages installed:

		make
		pkgconf
		libgtk-4-dev

# Support

   My Libera IRC support channel: #linuxfriends
   
   Email: nicolas.dio@protonmail.com

