This project is EXTREMELY unfinished, but i hope it shows how my coding ability has developed over the past (roughly year)

Due to restructuring and improvements to the way that this project has been strucutred, most of the code within the client/ and server/ source code directories is all depreciated. All of my up to date code is stored in lib/  the files that i have coded are all prefixed with "crbn_". (apart from crbn_imageDownload which was mostly created via generative ai)


I have created this project entirely in an offline git repository, but at the moment i canont upload the git repository due to private api keys being exposed to the internet in almost all of the more recent commits, which is why i have this copy paste repo.

A lot of the code is coded in quite a dangerous way with incorrect use of raw pointers but i have since learned about ways to mitigate these dangers. I plan on fixing and updating all of these dangers with the use of smart pointers and other scoped methods of managing data


This project sadly will only compile easily on linux using the builds/buildALL.sh script. Its dependencies can be installed on debian based distros with 

sudo apt install nlohmann-json3-dev curl

Other libraries i am using include : asio-1.30.2, stb_image.h, olcPixelGameEngine.h but these are all included as header files.
