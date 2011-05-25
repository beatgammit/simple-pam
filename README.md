Intro
=====

This is just a simple PAM module and test code for it. There really isn't much to it, but it does make a good example of how to get started with a PAM module.

To build, either use the build scripts or use these commands:

**Build the PAM module**

`gcc -fPIC -fno-stack-protector -c src/mypam.c`

`sudo ld -x --shared -o /lib/security/mypam.so mypam.o`

The first command builds the object file in the current directory and the second links it with PAM. Since it's a shared library, PAM can use it on the fly without having to restart.

**Build Test**

`g++ -o pam_test src/test.c -lpam -lpam_misc`

OR

`gcc -o pam_test src/test.c -lpam -lpam_misc`

The test program is valid C, so it could be compiled using gcc or g++. I like g++ better because I'll probably want to extend it and I like C++ better.

Simple Usage
------------

The build scripts will take care of putting your module where it needs to be, `/lib/security`, so the next thing to do is edit config files.

The config files are located in `/etc/pam.d/` and the one I edited was `/etc/pam.d/common-auth`.

The test application tests auth and account functionality (although account isn't very interesting). At the top of the pam file (or anywhere), put these lines:

	auth sufficient mypam.so
	account sufficient mypam.so

I think the account part should technically go in `/etc/pam.d/common-account`, but I put mine in the same place so I'd remember to take them out later.

To run the test program, just do: `pam_test backdoor` and you should get some messages saying that you're authenticated! Maybe this is how Sam Flynn 'hacked' his father's computer in TRON Legacy =D.

Resources
=========

I found these resources especially helpful:

O'Reilly Guides:
----------------

These guides give brief overviews about PAM and how to write modules.  This is useful if you already have a little knowledge.

* [Writing PAM Modules, Part One](http://linuxdevcenter.com/pub/a/linux/2002/05/02/pam_modules.html)
* [Writing PAM Modules, Part Two](http://linuxdevcenter.com/pub/a/linux/2002/05/23/pam_modules.html)
* [Writing PAM Modules, Part Three](http://linuxdevcenter.com/pub/a/linux/2002/05/30/pam_modules.html)

Others
------

Good example for simple authentication.  I adapted this one in my simple PAM module.

[2-factor authentication & writing PAM modules](http://ben.akrin.com/?p=1068)

Gives an example program that uses PAM. I adapted this for testing my PAM module.

[Example PAM application](http://www.kernel.org/pub/linux/libs/pam/Linux-PAM-html/adg-example.html)
