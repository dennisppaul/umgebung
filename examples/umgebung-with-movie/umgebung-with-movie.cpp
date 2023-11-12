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
        if (myMovie->available()) {
            myMovie->read();
        }
        image(myMovie, mouseX, mouseY);
    }

    void keyPressed() {
        if (key == 'Q') {
            exit();
        }
    }
};

PApplet *instance() {
    return new UmgebungExampleAppWithMovie();
}