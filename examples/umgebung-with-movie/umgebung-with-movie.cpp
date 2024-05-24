#include "Umgebung.h"
#include "Movie.h"

using namespace umgebung;

class UmgebungExampleAppWithMovie : public PApplet {

    Movie* myMovie;

    void settings() {
        size(1024, 768);
    }

    void setup() {
        myMovie = new Movie("../video.mp4");
        myMovie->play();
        myMovie->loop();
    }

    void draw() {
        background(0);
        //        if (myMovie->available()) {
        //            myMovie->read();
        //        }
        myMovie->reload(); // TODO if run from thread ( i.e `myMovie->play();` ) this needs to be called in draw
        image(myMovie, mouseX, mouseY);
    }

    void keyPressed() {
        if (key == 'q') {
            exit();
        }
        if (key == 'p') {
            myMovie->play();
        }
        if (key == 's') {
            myMovie->pause();
        }
    }
};

PApplet* umgebung::instance() {
    return new UmgebungExampleAppWithMovie();
}