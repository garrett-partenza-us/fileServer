#!/bin/sh

. config.ini

echo $usb1
echo $usb2

echo "Creating temprorary directory..."
temp_folder="local"
mkdir $temp_folder

sleep 1

file1="helloworld.txt"
echo "Hello World" > "$temp_folder/$file1"

sleep 1

./fget PUT "$temp_folder/$file1" "$file1"

sleep 1

./fget INFO "$file1"

sleep 1

subdir="dir1"
./fget MD "$subdir"

sleep 1

echo "Cleaning up..."

./fget RM "$subdir"
./fget RM "$file1"
rm -r "$temp_folder"