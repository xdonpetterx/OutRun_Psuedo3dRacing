#include <SFML/Graphics.hpp>
#include <cmath>

int width = 1280;
int height = 720;
int roadW = 2000;
int segL = 200; //segment length
float camD = 0.84; //camera depth

struct Line {
    float x, y, z; //3d center of line
    float X, Y, W; //screen coord
    float scale, curve, spriteX, clip;
    sf::Sprite sprite;

    Line(){curve=x=y=z=0;}

    //from world to screen coordinates
    void project(int camX, int camY, int camZ){
        scale = camD/(z-camZ);
        X = (1 + scale*(x - camX)) * width/2;
        Y = (1 - scale*(y - camY)) * height/2;
        W = scale * roadW  * width/2;
    }

    void drawSprite(sf::RenderWindow& app){
        sf::Sprite s = sprite;
        int w = s.getTextureRect().width;
        int h = s.getTextureRect().height;

        float destX = X + scale * spriteX * width/2;
        float destY = Y + 4;
        float destW = w * W / 266;
        float destH = h * W / 266;

        destX += destW * spriteX; //offsetX
        destY += destH * (-1); //offsetY

        float clipH = destY + destH - clip;
        if (clipH < 0) clipH = 0;

        if (clipH >= destH) return;
        s.setTextureRect(sf::IntRect(0, 0, w, h-h*clipH/destH));
        s.setScale(destW/w, destH/h);
        s.setPosition(destX, destY);
        app.draw(s);
    }

};

void drawQuad(sf::RenderWindow &w, sf::Color c, int x1, int y1, int w1, int x2, int y2, int w2){
    sf::ConvexShape shape(4);
    shape.setFillColor(c);
    shape.setPoint(0, sf::Vector2f(x1-w1, y1));
    shape.setPoint(1, sf::Vector2f(x2-w2, y2));
    shape.setPoint(2, sf::Vector2f(x2+w2, y2));
    shape.setPoint(3, sf::Vector2f(x1+w1, y1));
    w.draw(shape);
}

int main()
{
    sf::RenderWindow app(sf::VideoMode(width, height), "Outrun Racing!");
    app.setFramerateLimit(60);

    sf::Texture bg;
    bg.loadFromFile("../images/bg.png");
    bg.setRepeated(true);
    sf::Sprite sBackground(bg);

    sf::Texture t;
    t.loadFromFile("../images/5.png");
    sf::Sprite sTree(t);

    std::vector<Line> lines;

    for (int i = 0; i < 1600; i++) {
        Line line;
        line.z = i * segL;

        if (i > 300 && i < 700) line.curve = 0.5;

        if (i > 750) line.y = sin(i/30.0) * 1500;

        if (i % 20 == 0) {line.spriteX = -2.5; line.sprite = sTree;}

        lines.push_back(line);
    }

    int N = lines.size();
    int pos = 0;
    int playerX = 0;

    if (sf::Joystick::isConnected(0)) {
        printf("Controller connected\n");
        printf("The controller has %d buttons.\n", sf::Joystick::getButtonCount(0));
    }
    else
        printf("Controller disconnected\n");


    while (app.isOpen()){
        sf::Event e;
        while (app.pollEvent(e)){
            if (e.type == sf::Event::Closed)
                app.close();
        }

//        In SFML:
//        POVx, POVy are probably the d-pad.
//                X, Y are probably the left most controller
//        Z, R are either the right joystick or the triggers.
//                U, V are whichever one Z, R isn't.

        //button 0 = a on xbox, x on ps5
        //xbox button 1 = b
        //xbox button 2 = x on xbox, triangle on ps5
        //xbox button 3 = y
        //xbox button 4 = lb
        //xbox button 5 = rb
        //xbox button 6 = back
        //xbox button 7 = start
        //xbox button 8 = xbox button
        //xbox button 9 = left stick click
        //xbox button 10 = right stick click
        //xbox axis x > 0 = left stick to the right
        //xbox axis x < 0 = left stick to the left
        //xbox axis y > 0 = left stick down
        //xbox axis y < 0 = left stick up
        //xbox axis z > 0 = right trigger pressed (probably)
        //xbox axis z < 0 = left trigger pressed (probably)
        //xbox axis PovX > 0 = d-pad to the right
        //xbox axis PovX < 0 = d-pad to the left



        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) playerX += 200;
        if (sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X) > 30) playerX += 200;
        if (sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovX) > 0) playerX += 200;
        //if ((sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Z)) < 99) pos += 200;
        //if (sf::Joystick::isConnected(0) && sf::Joystick::getAxisPosition(0, sf::Joystick::X) > 10) playerX += 200;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) playerX -= 200;
        if (sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X) < -30) playerX -= 200;
        if (sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovX) < 0) playerX -= 200;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) pos+=200;
        if (sf::Joystick::isButtonPressed(0, 0)) pos += 200;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) pos-=200;
        if (sf::Joystick::isButtonPressed(0, 2)) pos -= 200;

        while (pos >= N*segL) pos -= N*segL;
        while (pos < 0) pos += N*segL;

        app.clear(sf::Color::Black);
        int startPos = pos/segL;
        int camH = lines[startPos].y + 1500;
        float x= 0, dx = 0;
        int maxy = height;

        //////////////////draw road//////////////////////
        for (int n = startPos; n < startPos + 300; n++) {
            Line& l = lines[n % N];
            l.project(playerX - x, camH, pos - (n>=N?N*segL:0));
            x+=dx;
            dx+=l.curve;

            if (l.Y >= maxy) continue;
            maxy = l.Y;

            sf::Color grass = (n / 3) % 2 ? sf::Color(16, 200, 16) : sf::Color(0, 154, 0);
            sf::Color rumble = (n / 3) % 2 ? sf::Color(255, 255, 255) : sf::Color(0, 0, 0);
            sf::Color road = (n / 3) % 2 ? sf::Color(107, 107, 107) : sf::Color(105, 105, 105);

            Line p = lines[(n - 1) % N]; //previous line

            drawQuad(app, grass, 0, p.Y, width, 0, l.Y, width);
            drawQuad(app, rumble, p.X, p.Y, p.W * 1.2, l.X, l.Y, l.W * 1.2);
            drawQuad(app, road, p.X, p.Y, p.W, l.X, l.Y, l.W);
        }

        //drawQuad(app, sf::Color::Green, 500, 500, 200, 500, 300, 100);
        //drawQuad(app, sf::Color::White, 0, 500, 5, 0, 600, 5);
        //drawQuad(app, sf::Color::White, 1000, 500, 5, 1000, 600, 5);

        //////////////////draw objects//////////////////////
        for (int n = startPos + 300; n > startPos; n--)
            lines[n % N].drawSprite(app);

        app.display();
    }

    return 0;
}