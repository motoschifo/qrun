#!/bin/bash

# defaults
FILE_MANAGER="dolphin"
TERMINAL="bash -i -c"
#TERMINAL="konsole --noclose -e bash -c"

# Display the KDE input dialog box
CMD=$(/usr/bin/kdialog --title "Quick Run" --inputbox "Enter the command or a folder name:" "")

# Exit is the command is empty
if [ $? -ne 0 ] || [ -z "$CMD" ]; then
    exit
fi

if [ -d "${CMD}" ]; then
    # It's a volume folder
    ${FILE_MANAGER} "${CMD}" &
elif [ -d "${HOME}/${CMD}" ]; then
    # it's a local user folder
    ${FILE_MANAGER} "${HOME}/${CMD}" &
else
    # it's a script or application
    ${TERMINAL} "$CMD" &
fi
