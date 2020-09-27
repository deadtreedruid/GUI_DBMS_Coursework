# Component-Based GUI-driven DBMS software
Please note before reading this, it was entirely written in 6 days during my second year of uni and has numerous problems that I'll detail here:
* Rendering, event handling and logic are handled on one thread, so the performance isn't great
* Screen elements inexplicably don't inherit from the base GUI element class but re-implement most of that class' features
* Lots of stuff is hardcoded and would need a significant refactor to work for other projects (which I'm currently working on!)
* The particle stuff is really out of place but I guess I thought it looked cool?
* Globals all over the place since I needed to get it done quickly
* Lots of new and deleting since shared_ptr was/is too slow and resource consuming for a complex single-threaded app

![pic](https://i.imgur.com/8ZMPfcL.png)

![pic2](https://i.imgur.com/bHhG2fB.png)

![pic2](https://i.imgur.com/ezoqErt.png)
