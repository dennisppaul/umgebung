#include "Umgebung.h"
#include "Movie.h"

class UmgebungExampleAppWithMovie : public PApplet {

    Movie *myMovie;

    void settings() {
        size(1024, 768);
    }

    void setup() {
        myMovie = new Movie("../video.mp4");
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
        if (key == 'Q') {
            exit();
        }
        if (key == 'P') {
            myMovie->play();
        }
        if (key == 'S') {
            myMovie->pause();
        }
    }
};

PApplet *instance() {
    return new UmgebungExampleAppWithMovie();
}