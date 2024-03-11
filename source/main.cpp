#include <iostream>
#include <vector>
#include <cmath>
#include <GLFW/glfw3.h>

const int WIDTH = 800;
const int HEIGHT = 600;
const int MIN_SHAPES = 100;



struct Shape {
    float x, y; // Position
    float dx, dy; // Velocity
    float size; // Size of the shape
    float r, g, b; // Color

    Shape(float _x, float _y, float _dx, float _dy, float _size, float _r, float _g, float _b) :
        x(_x), y(_y), dx(_dx), dy(_dy), size(_size), r(_r), g(_g), b(_b) {}

    void move(float dt) {
        x += dx * dt;
        y += dy * dt;

        // Simple boundary check to keep shapes within the window
        if (x < 0 || x > WIDTH) {
            dx *= -1;
            x = std::max(0.0f, std::min(static_cast<float>(WIDTH), x));
        }
        if (y < 0 || y > HEIGHT) {
            dy *= -1;
            y = std::max(0.0f, std::min(static_cast<float>(HEIGHT), y));
        }
    }
};


std::vector<Shape> shapes;

void glfw_onKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_UP) {
            // Increase the number of shapes
            shapes.emplace_back(
                static_cast<float>(rand() % WIDTH),
                static_cast<float>(rand() % HEIGHT),
                static_cast<float>((rand() % 200) - 100),
                static_cast<float>((rand() % 200) - 100),
                static_cast<float>((rand() % 20) + 10),
                static_cast<float>(rand()) / RAND_MAX,
                static_cast<float>(rand()) / RAND_MAX,
                static_cast<float>(rand()) / RAND_MAX
            );
        }
        else if (key == GLFW_KEY_DOWN && shapes.size() > MIN_SHAPES) {
            // Decrease the number of shapes
            shapes.pop_back();
        }
    }
}

class CollisionResolver {
public:
    virtual void resolveCollisions(std::vector<Shape>& shapes) = 0;
    virtual ~CollisionResolver() {}
};

class DefaultCollisionResolver : public CollisionResolver {
public:
    void resolveCollisions(std::vector<Shape>& shapes) override {
        for (size_t i = 0; i < shapes.size(); ++i) {
            for (size_t j = i + 1; j < shapes.size(); ++j) {
                // Simple collision detection - bounding circle intersection
                float dx = shapes[j].x - shapes[i].x;
                float dy = shapes[j].y - shapes[i].y;
                float distance = sqrt(dx * dx + dy * dy);
                float minDist = shapes[i].size + shapes[j].size;

                if (distance < minDist) {
                    // Swap velocities for bouncing effect
                    std::swap(shapes[i].dx, shapes[j].dx);
                    std::swap(shapes[i].dy, shapes[j].dy);
                }
            }
        }
    }
};

class Sandbox {
private:
    std::vector<Shape> shapes;
    CollisionResolver* resolver;

public:
    Sandbox(CollisionResolver* resolver) : resolver(resolver) {}

    void drawShape(float x, float y, float size, float r, float g, float b) {
        glBegin(GL_QUADS);
        glColor3f(r, g, b);
        glVertex2f(x - size, y - size);
        glVertex2f(x + size, y - size);
        glVertex2f(x + size, y + size);
        glVertex2f(x - size, y + size);
        glEnd();
    }

    void drawShapes() {
        for (const auto& shape : shapes) {
            drawShape(shape.x, shape.y, shape.size, shape.r, shape.g, shape.b);
        }
    }

    void updateShapes(float dt) {
        for (auto& shape : shapes) {
            shape.move(dt);
        }
    }

    void update(float dt) {
        updateShapes(dt);
        resolver->resolveCollisions(shapes);
    }

    

    void run() 
    {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return;
        }

        GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Shapes Sandbox", nullptr, nullptr);
        if (!window) {
            std::cerr << "Failed to create window" << std::endl;
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(window);
        glfwSetKeyCallback(window, glfw_onKeyPressed);
        glViewport(0, 0, WIDTH, HEIGHT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, WIDTH, HEIGHT, 0, 0, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        // Generate some random shapes
        for (int i = 0; i < MIN_SHAPES; ++i) {
            shapes.emplace_back(
                static_cast<float>(rand() % WIDTH),
                static_cast<float>(rand() % HEIGHT),
                static_cast<float>((rand() % 200) - 100),
                static_cast<float>((rand() % 200) - 100),
                static_cast<float>((rand() % 20) + 10),
                static_cast<float>(rand()) / RAND_MAX,
                static_cast<float>(rand()) / RAND_MAX,
                static_cast<float>(rand()) / RAND_MAX
            );
        }

        double lastTime = glfwGetTime();
        int frameCount = 0;
        while (!glfwWindowShouldClose(window)) {
            double currentTime = glfwGetTime();
            double deltaTime = currentTime - lastTime;
            lastTime = currentTime;

            ++frameCount;
            if (currentTime >= 1.0) {
                std::cout << "FPS: " << frameCount << " | Number of Shapes: " << shapes.size() << std::endl;
                frameCount = 0;
                lastTime += 1.0;
            }

            glfwPollEvents();
            update(static_cast<float>(deltaTime));
            drawShapes();
        }

        glfwTerminate();
    }
};

int main() {
    DefaultCollisionResolver defaultResolver;
    Sandbox sandbox(&defaultResolver);
    sandbox.run();
    return 0;
}
