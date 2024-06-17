////#pragma once
//#define _USE_MATH_DEFINES
//
//#include <SFML/Graphics.hpp>//
//#include <vector>//
//#include <cmath>//
//#include <iostream>
//#include <algorithm>//
//#include <tuple>
//
//#include "linearAlgebra.h"//
//#include "drawingTools.h"//
//#include "Track.h"//
//#include "Spring.h"//
//#include "Body.h"
//
//
//// Global Variables defined here, Instance variables for objects defined in main()
//double t = 0.0;
//const double baseLine_dt = 0.002;
//double dt = baseLine_dt;
//const double g = 30.0;
//const double collisionTolConst = 1.0001;
//double collisionTol = 0.000001;
//const double lowestCollisionPossible = 0.5;
//const int maxNumRollbacks = 100;
//const double windowDimension = 800;
//double drawingScaleFactor = 1.0;
//
//sf::RenderWindow window(sf::VideoMode(windowDimension, windowDimension), "Body on Track");
//
//void showStartingState(Body body, std::vector<Spring> springs, Track track) {
//    body.update();
//    track.draw(window);
//    body.draw(window);
//    for (Spring spring : springs) {
//        spring.draw(window, body);
//    }
//    window.display();
//    sf::Time initialWaitTime = sf::milliseconds(1000);
//    sf::sleep(initialWaitTime);
//}
//
//void simulate(Body body, std::vector<Spring> springs, Track track, double finalTime, double fps) {
//    double frameTimeMilisec = 1000.0 / fps;
//    sf::Clock clock;
//    int frame = 0;
//    while (t < finalTime) {
//        window.clear();
//        body.update();
//        track.draw(window);
//        body.draw(window);
//        for (Spring spring : springs) {
//            spring.draw(window, body);
//        }
//        t = t + dt;
//        if (t > frame * dt){
//            dt = baseLine_dt;
//            window.display();
//            sf::Time elapsed = clock.getElapsedTime();
//            sf::Time sleepTime = sf::milliseconds(frameTimeMilisec) - elapsed;
//            if (sleepTime > sf::Time::Zero) {
//                sf::sleep(sleepTime);
//            }
//            clock.restart();
//            frame++;
//        }
//    }
//}
//
//int main() {
//
//    // initialize objects for simulation
//    // first initialize track
//    double Ti = -2.3;
//    double Tf = 2.3;
//    int numSplineLines = 3000;
//    Track track(Ti, Tf, numSplineLines);
//    collisionTol = collisionTolConst * track.getLargestS() * 0.5;
//    double xMin = track.getXMin();
//    double xMax = track.getXMax();
//    double yMin = track.getYMin();
//    double yMax = track.getYMax();
//    drawingScaleFactor = setupWindowScaling(xMin, xMax, yMin, yMax, window);
//
//    // second initialize springs
//    double xA = -1.5;
//    double yA = 0.0;
//    double restLen = 1.0; // spring rest length
//    double stiffness = 200.0; // spring stiffness
//    double anchorSidePixels = 20;
//    Spring spring(xA, yA, restLen, stiffness, anchorSidePixels);
//    std::vector<Spring> springs;
//    springs.push_back(spring);
//
//    // third initialize body
//    double startingProportion = 0.01;
//    double s = (track.getSValues()[numSplineLines]) * startingProportion;
//    double v = 0.0;
//    double x = 0.0;
//    double y = 0.0;
//    double vx = 0.0;
//    double vy = 0.0;
//    double m = 10.0;
//    double elasticity = 0.75; // value (0 to 1) affects energy lost upon collission
//    double stickyness = 0.1; // value (0 to 1) affects ease of body sticking to track
//    double friction = 2; // friction coefficient //Need to debug both friction and drag
//    double drag = 0.3; // drag coefficient
//    double radiusPixels = 8.0;
//    bool onTrack = true;
//    Body body(s, v, x, y, vx, vy, m, elasticity, stickyness, friction, drag, radiusPixels, onTrack, track, springs);
//
//    // display frame one of simulation animation
//    showStartingState(body, springs, track);
//
//    // run the remaining simulation animation
//    double finalTime = 100.0;
//    double fps = 60.0;
//    simulate(body, springs, track, finalTime, fps);
//
//    return 0;
//}
//
//
