#!/bin/bash

PROGRAM_NAME="HueControllerQT"
INSTALL_DIR="/usr/local/bin"
CUR_USER=$(whoami)
CUR_USER_HOME=$HOME

test -f supervisor.conf.template || { echo "No supervisor.conf found. Run in the install folder."; exit 1; }

#################################################################
#               Get the app and install it
#################################################################

test -f $PROGRAM_NAME || { echo "Get the software somehow"; exit 1; }

test -f $INSTALL_DIR/$PROGRAM_NAME && { echo "Already found $PROGRAM_NAME at $INSTALL_DIR. Overwriting."; }

sudo cp $PROGRAM_NAME $INSTALL_DIR

update_config_template() {

    template_file=$1
    output_file=$2

    # set the correct app paths to template
    sed "s:/path/to/bin:${INSTALL_DIR}:g" $template_file > $output_file

    # set the app name
    sed -i "s:my-app-name:${PROGRAM_NAME}:g" $output_file

    # set current user
    sed -i "s:/my/user:${CUR_USER}:g" $output_file

    # set current user home
    sed -i "s:/my/home/dir:${CUR_USER_HOME}:g" $output_file

}

#################################################################
#       Setup app to start at startup
#################################################################

#sudo apt install supervisor

DESKTOP_FILE_TEMPLATE="HueControllerQT.desktop.template"
DESKTOP_FILE="HueControllerQT.desktop"

update_config_template $DESKTOP_FILE_TEMPLATE $DESKTOP_FILE

# copy the template configuration
mkdir -p $CUR_USER_HOME/.config/autostart
sudo cp $DESKTOP_FILE $CUR_USER_HOME/.config/autostart/$DESKTOP_FILE




#################################################################
#       Setup supervisor to start app at startup
#################################################################

#sudo apt install supervisor

#update_config_template "supervisor.conf.template" "supervisor.conf"

# copy the template configuration
#test -d /etc/supervisor/conf.d || { echo "No configuration folder /etc/supervisor/conf.d existing. Not sure if should create one..."; exit 1; }
#sudo cp supervisor.conf /etc/supervisor/conf.d/$PROGRAM_NAME.conf

# initialize logfiles
#sudo mkdir -p /var/log/$PROGRAM_NAME
#sudo touch /var/log/$PROGRAM_NAME/error.log
#sudo touch /var/log/$PROGRAM_NAME/server.log

#sudo supervisorctl reload



