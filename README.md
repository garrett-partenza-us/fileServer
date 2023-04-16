# Practicum II 
This project implements a simple RAID 1 file server using TCP traffic and two USB flash drives. My implementation also handles USB device failures by a hearbeat program which syncronizes failed devices coming back online.


### Instructions

###### NOTE: Before following these steps, make sure that you have two USB devices plugged into your computer, and update the config.ini file to reflect the paths to these two USBS. You can also configure the RAID server's IP and PORT number.

1. Compile code
```
make clean
make 
```
2. Open three seperate terminals (one for client, server, and hearbeat monitor)
3. Start server
```
./fget-server
```
4. Start hearbeat monitor
```
./heartbeat
```
5. Run tests
```
chmod 777 test.sh
./test.sh
```
6. (Optional) Print help
```
./fget
```
7. (Optional) Export to PATH
```
export PATH=$PATH:<path/to/fget> 
```
8. (Optional) Custom commands
```
./fget [OPERATION] [ARGS]
```
