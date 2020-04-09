
//
// Disclaimer:
// ----------
//
// This code will work only if you selected window, graphics and audio.
//
// In order to load the resources like cute_image.png, you have to set up
// your target scheme:
//
// - Select "Edit Scheme…" in the "Product" menu;
// - Check the box "use custom working directory";
// - Fill the text field with the folder path containing your resources;
//        (e.g. your project folder)
// - Click OK.
//

#include <iostream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

class Window {
private:
    sf::RenderWindow window;
    sf::Vector2u windowSize;
    std::string title;
    bool closeWindow;
    bool fullScreen;
    void clear() {window.close();}
    void setup(const std::string &title, const sf::Vector2u &size);
    void create();
    
public:
//    Window() {setup("Default", sf::Vector2u(600, 800));}
    Window(std::string title, sf::Vector2u windowSize) {setup(title, windowSize);}
    ~Window() {clear();}
    void clearDraw() {window.clear(sf::Color::White);}
    void draw(sf::Drawable &drawable) {window.draw(drawable);}
    void display() {window.display();}
    void Update();
    void setCloseWindow() {closeWindow = true;}
    bool getCloseWindow() {return closeWindow;}
    bool getFullScreen() {return fullScreen;}
    sf::Vector2u getWindowSize() {return windowSize;}
    sf::RenderWindow* getWindow() {return &window;}
    void toggleFullScreen();
};

void Window::setup(const std::string &title, const sf::Vector2u &size) {
    this->title = title;
    this->windowSize = size;
    closeWindow = false;
    fullScreen = false;
    create();
}

void Window::create() {
    auto style = (fullScreen ? sf::Style::Fullscreen : sf::Style::Default);
    window.create({windowSize.x, windowSize.y, 32}, title, style);
}

void Window::Update() {
    sf::Event e;
    while(window.pollEvent(e)) {
        if (e.type == sf::Event::Closed) closeWindow = true;
    }
}

void Window::toggleFullScreen() {
    fullScreen = !fullScreen;
    clear();
    create();
}

struct Node {
    int data;
    int index;
    Node() : data(0), index(0){}
};

struct Bar {
    int data;
    int x;
    Bar(int data, int posX) : data(data), x(posX){}
};

class Algorithm {
private:
    Node lastPos;
    Node cursor;
    int minIndex;
    int dataSize;
    bool isSort;    // true if dataset is sorted
    bool needSwap;  // true if we need to swap data
    std::pair<int, int> swapPos;
    
public:
    Algorithm() : lastPos(), cursor(), dataSize(0), minIndex(0), isSort(false), swapPos(0,0){}
    ~Algorithm(){}
    void selection(std::vector<Bar> &data);
    void setSize(int dataSize) {this->dataSize = dataSize;}
    bool getNeedSwap() {return needSwap;}
    bool getIsSort() {return isSort;}
    void swapBar(std::vector<Bar> &dataset, int index1, int index2);
    void drawBar(sf::RenderWindow &window, int data, int posX, sf::Color color);
    void render(sf::RenderWindow &window, std::vector<Bar> &dataset);
};

void Algorithm::selection(std::vector<Bar> &dataset) {
    if (lastPos.index == dataset.size() - 1) {
        isSort = true;
        return;
    }
    if (lastPos.index == 0 && cursor.index == 0) {
        lastPos.data = dataset[0].data;
        cursor.data = lastPos.data;
    }
    if (!needSwap) {
        cursor.index++;
        cursor.data = dataset[cursor.index].data;
        if (dataset[minIndex].data > cursor.data) minIndex = cursor.index;
        if (cursor.index == dataset.size() - 1) {
//            swap data
            swapPos.first = dataset[lastPos.index].x;
            swapPos.second = dataset[minIndex].x;
            needSwap = true;
            lastPos.index++;
            lastPos.data = dataset[lastPos.index].data;
        }
    } else if (needSwap){
        swapBar(dataset, lastPos.index - 1, minIndex);
        if (dataset[lastPos.index - 1].x >= swapPos.second) {
            dataset[lastPos.index - 1].x = swapPos.second;
            dataset[minIndex].x = swapPos.first;
            needSwap = false;
            std::swap(dataset[lastPos.index - 1], dataset[minIndex]);
            minIndex = lastPos.index;
            cursor = lastPos;
        }
    }
}

void Algorithm::swapBar(std::vector<Bar> &dataset, int index1, int index2) {
//    swapping speed
    dataset[index1].x += 25;
    dataset[index2].x -= 25;
}

void Algorithm::drawBar(sf::RenderWindow &window, int data, int posX, sf::Color color) {
    int posY = 900, ratio = 5;
    sf::RectangleShape bar;
    bar.setSize(sf::Vector2f(25, data * ratio));
    bar.setPosition(posX, posY - data * ratio);
    bar.setFillColor(color);
    window.draw(bar);
}

void Algorithm::render(sf::RenderWindow &window, std::vector<Bar> &dataset) {
    sf::Color darkRed(184, 4, 40);
    if (needSwap) {
        drawBar(window, dataset[lastPos.index - 1].data, dataset[lastPos.index - 1].x, darkRed);
        drawBar(window, dataset[minIndex].data, dataset[minIndex].x, sf::Color(0, 204, 88));
        drawBar(window, dataset[cursor.index].data, dataset[cursor.index].x, darkRed);
    } else {
        drawBar(window, dataset[lastPos.index].data, dataset[lastPos.index].x, darkRed);
        drawBar(window, dataset[cursor.index].data, dataset[cursor.index].x, darkRed);
        if (lastPos.index != minIndex)
            drawBar(window, dataset[minIndex].data, dataset[minIndex].x, sf::Color(0, 204, 88));
    }
}

class Selection : public Algorithm {
private:
    Node lastPos;
    Node cursor;
    int minIndex;
    std::pair<int, int> swapPos;
    
public:
    Selection();
    ~Selection(){}
    void Update();
    void sorting(std::vector<Bar> &data);
    void render(sf::RenderWindow &window, std::vector<Bar> &dataset);
};

class Sorting {
private:
    Window window;
    Algorithm alg;
    std::vector<Bar> dataset;
    int dataSize;
    int speed;
    int sortAlg;
    bool isPause;
    enum KEY {NONE, PAUSE, SELECTION, INSERTION, BUBBLE,
        QUICK, MERGE, SHELL};
    void handleEvent();
    sf::Clock clock;
    float elapsed;
public:
    Sorting();
    Sorting(int dataSize);
    ~Sorting(){}
    void Update();
    void handleInput();
    bool closeWindow() {return window.getCloseWindow();}
    void render();
    sf::Time getElapsed() {return clock.getElapsedTime();}
    void restartClock() {elapsed += clock.restart().asSeconds();}
};

void Sorting::handleEvent() {
    if (sortAlg != NONE) {
        if (sortAlg == SELECTION && !alg.getIsSort()) alg.selection(dataset);
        else if (alg.getIsSort()) {
            sortAlg = NONE;
//            for (int i = 0; i < dataset.size(); i++)
//                std::cout << dataset[i].data << std::endl;
        }
    }
}

Sorting::Sorting() : window("Sorting", sf::Vector2u(1200,1200)), alg(){
    dataSize = 20;
    speed = 10;
    alg.setSize(dataSize);
    sortAlg = NONE;
    clock.restart();
    elapsed = 0.0f;
    isPause = false;
    srand(time(nullptr));
    for (int i = 0, posX = 60; i < dataSize; i++, posX += 30)
        dataset.push_back(Bar(rand() % 100 + 1, posX));
}

void Sorting::Update() {
    window.clearDraw();
    handleInput();
//    algorithm speed
    float timestep = 1.0f / speed;
    if (elapsed >= timestep) {
        if (!isPause) {
            handleEvent();
        }
        elapsed -= timestep;
    }
    render();
    window.display();
}

void Sorting::handleInput() {
    sf::Event e;
    window.clearDraw();
    while (window.getWindow()->pollEvent(e)) {
        if (e.type == sf::Event::Closed) window.setCloseWindow();
        if (e.type == sf::Event::KeyPressed) {
            if (e.key.code == sf::Keyboard::Num1) sortAlg = SELECTION;
            if (e.key.code == sf::Keyboard::Num2) sortAlg = INSERTION;
            if (e.key.code == sf::Keyboard::Num3) sortAlg = BUBBLE;
            if (e.key.code == sf::Keyboard::Num4) sortAlg = QUICK;
            if (e.key.code == sf::Keyboard::Num5) sortAlg = MERGE;
            if (e.key.code == sf::Keyboard::Num6) sortAlg = SHELL;
            if (e.key.code == sf::Keyboard::Space) isPause = !isPause;
            if (e.key.code == sf::Keyboard::Right) {
                speed -= 1;
                if (speed <= 0) speed = 1;
            }
            if (e.key.code == sf::Keyboard::Left) speed += 1;
        }
    }
}

void Sorting::render() {
    sf::RectangleShape bar;
    std::vector<Bar>::iterator iter;
    for (iter = dataset.begin(); iter != dataset.end(); ++iter)
        alg.drawBar(*window.getWindow(), iter->data, iter->x, sf::Color(102, 181, 255));
    if (sortAlg == SELECTION) alg.render(*window.getWindow(), dataset);
}

int main(int argc, char const** argv)
{
    Sorting game;
    while (!game.closeWindow()) {
        game.Update();
        game.restartClock();
    }
    return EXIT_SUCCESS;
}
