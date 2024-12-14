#include "ProjectileMotion.h"
#include "Ball.h"

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

//-------------------------------------------------------------------------------------------------
// Enumerations and Constants
//-------------------------------------------------------------------------------------------------
enum ActiveField {
    kNoActiveField,
    kSpeedField,
    kGravityField
    // Note: Angle field is read-only
};

// Initial states and defaults
static const float kWindowWidth = 1920.f;
static const float kWindowHeight = 1080.f;
static const float kFrameRateLimit = 60.f;

static const float kGroundLineY = 800.f;         // Y coordinate for "ground"
static const float kCharacterInitialX = 118.f;   // Initial 'x' position of character sprite
static const float kCartInitialX = 1550.f;       // Initial 'x' position of cart sprite

static const float kDefaultSpeed = 11.5f;        // Default launch speed (m/s)
static const float kDefaultGravity = 9.8f;       // Default gravity (m/s^2)
static const float kDefaultAngleDeg = 45.0f;     // Default launch angle (degrees)

static const float kScale = 100.f;               // Pixel-to-meter scale factor

static const float kTextFieldWidth = 150.f;
static const float kTextFieldHeight = 40.f;

//-------------------------------------------------------------------------------------------------
// Local Helper Lambda for Parsing
//-------------------------------------------------------------------------------------------------
static auto ParseFloat = [](const std::string& str, float default_val) -> float {
    try {
        return std::stof(str); // Checks if string can be float
    }
    catch (...) {
        return default_val; // Refer to a backup value
    }
    };

//-------------------------------------------------------------------------------------------------
// Function: runProjectileMotionSimulation
//
// Description:
//     Main function to run the projectile motion simulation. Handles loading assets, setting up
//     the window, initializing UI elements, processing events, running the simulation, and rendering.
//
// Parameters:
//     None
//
// Returns:
//     void
//-------------------------------------------------------------------------------------------------
void runProjectileMotionSimulation() {
    //-----------------------------------------------------------------------------
    // Window & Rendering Setup
    //-----------------------------------------------------------------------------
    sf::RenderWindow window(sf::VideoMode(static_cast<unsigned int>(kWindowWidth),
        static_cast<unsigned int>(kWindowHeight)),
        "Projectile Motion Simulator",
        sf::Style::Close);
    window.setFramerateLimit(static_cast<unsigned int>(kFrameRateLimit));

    //-----------------------------------------------------------------------------
    // Load Font
    //-----------------------------------------------------------------------------
    sf::Font font;
    if (!font.loadFromFile("retrogaming.ttf")) {
        std::cerr << "Error: Could not load font 'retrogaming.ttf'.\n";
        return;
    }

    //-----------------------------------------------------------------------------
    // Load Textures & Sprites
    //-----------------------------------------------------------------------------
    sf::Texture background_texture;
    if (!background_texture.loadFromFile("src/MotionInDimensions/imgs/background.jpg")) {
        std::cerr << "Error: Could not load 'background.jpg'.\n";
        return;
    }

    sf::Texture character_texture;
    if (!character_texture.loadFromFile("src/MotionInDimensions/imgs/character.png")) {
        std::cerr << "Error: Could not load 'character.png'.\n";
        return;
    }

    sf::Texture ball_texture;
    if (!ball_texture.loadFromFile("src/MotionInDimensions/imgs/ball.png")) {
        std::cerr << "Error: Could not load 'ball.png'.\n";
        return;
    }

    sf::Texture cart_texture;
    if (!cart_texture.loadFromFile("src/MotionInDimensions/imgs/cart.png")) {
        std::cerr << "Error: Could not load 'cart.png'.\n";
        return;
    }

    // Create background sprite and scale to window size
    sf::Sprite sprite_background(background_texture);
    {
        sf::Vector2u texture_size = background_texture.getSize();
        sf::Vector2u window_size = window.getSize();
        float scale_x = static_cast<float>(window_size.x) / texture_size.x;
        float scale_y = static_cast<float>(window_size.y) / texture_size.y;
        sprite_background.setScale(scale_x, scale_y);
    }

    // Set up character sprite
    sf::Sprite sprite_character(character_texture);
    {
        sf::FloatRect char_bounds = sprite_character.getLocalBounds();
        sprite_character.setOrigin(char_bounds.width / 2.f, char_bounds.height / 2.f);
        sprite_character.setPosition(kCharacterInitialX, kGroundLineY);
    }

    // Set up cart (target) sprite
    sf::Sprite sprite_cart(cart_texture);
    {
        sf::FloatRect cart_bounds = sprite_cart.getLocalBounds();
        sprite_cart.setOrigin(cart_bounds.width / 2.f, cart_bounds.height / 2.f);
        sprite_cart.setPosition(kCartInitialX, kGroundLineY);
    }

    //-----------------------------------------------------------------------------
    // UI Elements: Buttons
    //-----------------------------------------------------------------------------
    sf::RectangleShape simulate_button(sf::Vector2f(200.f, 60.f));
    simulate_button.setFillColor(sf::Color::Blue);
    simulate_button.setPosition(860.f, 900.f);

    sf::Text simulate_text("Simulate", font, 30);
    {
        sf::FloatRect btn_bounds = simulate_text.getLocalBounds();
        simulate_text.setOrigin(btn_bounds.width / 2.f, btn_bounds.height / 2.f);
        simulate_text.setPosition(simulate_button.getPosition().x + simulate_button.getSize().x / 2.f,
            simulate_button.getPosition().y + simulate_button.getSize().y / 2.f);
    }

    sf::RectangleShape reset_button(sf::Vector2f(200.f, 60.f));
    reset_button.setFillColor(sf::Color::Red);
    reset_button.setPosition(1080.f, 900.f);

    sf::Text reset_text("Reset", font, 30);
    {
        sf::FloatRect rt_bounds = reset_text.getLocalBounds();
        reset_text.setOrigin(rt_bounds.width / 2.f, rt_bounds.height / 2.f);
        reset_text.setPosition(reset_button.getPosition().x + reset_button.getSize().x / 2.f,
            reset_button.getPosition().y + reset_button.getSize().y / 2.f);
    }

    //-----------------------------------------------------------------------------
    // Simulation State Variables
    //-----------------------------------------------------------------------------
    bool simulation_running = false;
    bool goal_scored = false;
    bool out_of_bounds = false;
    bool ball_initialized = false;

    Ball volleyball(0, 0, 0, 0); // Will be initialized properly only once simulation starts

    sf::Text status_text;
    status_text.setFont(font);
    status_text.setCharacterSize(60);
    status_text.setFillColor(sf::Color::Yellow);
    status_text.setPosition(800.f, 500.f);

    //-----------------------------------------------------------------------------
    // Dragging States for Adjustment (Character, Cart, Angle)
    //-----------------------------------------------------------------------------
    bool dragging_character = false;
    bool dragging_cart = false;
    bool dragging_arrow = false;
    sf::Vector2f drag_offset;

    //-----------------------------------------------------------------------------
    // Parameters and UI Fields
    //-----------------------------------------------------------------------------
    std::string speed_str = "11.5";
    std::string gravity_str = "9.8";
    float arrow_angle = kDefaultAngleDeg; // degrees, with convention: 0°=Up, 90°=Right
    std::string angle_str = "45.0";

    sf::Vector2u window_size = window.getSize();

    // Speed input field
    sf::RectangleShape speed_field_rect(sf::Vector2f(kTextFieldWidth, kTextFieldHeight));
    speed_field_rect.setFillColor(sf::Color::White);
    speed_field_rect.setPosition(window_size.x - 200.f, 50.f);

    // Angle display field (read-only)
    sf::RectangleShape angle_field_rect(sf::Vector2f(kTextFieldWidth, kTextFieldHeight));
    angle_field_rect.setFillColor(sf::Color::White);
    angle_field_rect.setPosition(window_size.x - 200.f, 120.f);

    // Gravity input field
    sf::RectangleShape gravity_field_rect(sf::Vector2f(kTextFieldWidth, kTextFieldHeight));
    gravity_field_rect.setFillColor(sf::Color::White);
    gravity_field_rect.setPosition(window_size.x - 200.f, 190.f);

    // Field Labels
    sf::Text speed_label("Speed (m/s):", font, 20);
    speed_label.setFillColor(sf::Color::Black);
    speed_label.setPosition(window_size.x - 370.f, 50.f);

    sf::Text angle_label("Angle (deg):", font, 20);
    angle_label.setFillColor(sf::Color::Black);
    angle_label.setPosition(window_size.x - 370.f, 120.f);

    sf::Text gravity_label("Gravity (m/s^2):", font, 20);
    gravity_label.setFillColor(sf::Color::Black);
    gravity_label.setPosition(window_size.x - 410.f, 190.f);

    // Field Texts
    sf::Text speed_text(speed_str, font, 20);
    speed_text.setFillColor(sf::Color::Black);
    speed_text.setPosition(speed_field_rect.getPosition().x + 10.f, speed_field_rect.getPosition().y + 5.f);

    sf::Text angle_text(angle_str, font, 20); // Angle is read-only
    angle_text.setFillColor(sf::Color::Black);
    angle_text.setPosition(angle_field_rect.getPosition().x + 10.f, angle_field_rect.getPosition().y + 5.f);

    sf::Text gravity_text(gravity_str, font, 20);
    gravity_text.setFillColor(sf::Color::Black);
    gravity_text.setPosition(gravity_field_rect.getPosition().x + 10.f, gravity_field_rect.getPosition().y + 5.f);

    ActiveField active_field = kNoActiveField;

    //-----------------------------------------------------------------------------
    // Distance and Height Display
    //-----------------------------------------------------------------------------
    sf::Text distance_text("", font, 30);
    distance_text.setFillColor(sf::Color::Red);
    distance_text.setPosition(100.f, 100.f);

    sf::Text height_text("", font, 30);
    height_text.setFillColor(sf::Color::Red);
    height_text.setPosition(100.f, 150.f);

    //-----------------------------------------------------------------------------
    // Arrow Setup (used to visualize angle)
    //
    // arrow_angle definition:
    //   0° = Up, 90° = Right, 180° = Down, 270° = Left
    // The simulation angle is 90° - arrow_angle for projectile motion.
    //
    // The arrow is positioned relative to the character position.
    //-----------------------------------------------------------------------------
    sf::Vector2f character_arrow_offset(30.f, -175.f);
    sf::ConvexShape arrow_shape;
    arrow_shape.setPointCount(7);
    // Shape definition of arrow:
    // A simple arrow with a shaft and a pointed tip.
    arrow_shape.setPoint(0, sf::Vector2f(-2.5f, 0.f));      // bottom left
    arrow_shape.setPoint(1, sf::Vector2f(2.5f, 0.f));       // bottom right
    arrow_shape.setPoint(2, sf::Vector2f(2.5f, -100.f));    // shaft top right
    arrow_shape.setPoint(3, sf::Vector2f(10.f, -100.f));    // tip right base
    arrow_shape.setPoint(4, sf::Vector2f(0.f, -120.f));     // tip top
    arrow_shape.setPoint(5, sf::Vector2f(-10.f, -100.f));   // tip left base
    arrow_shape.setPoint(6, sf::Vector2f(-2.5f, -100.f));   // shaft top left
    arrow_shape.setFillColor(sf::Color::Red);

    // Platform Under Character 
    sf::RectangleShape platform_rect;
    platform_rect.setFillColor(sf::Color(139, 69, 19));
    float platform_width = 160.f;

    //-----------------------------------------------------------------------------
    // Lambda to reset simulation states
    //-----------------------------------------------------------------------------
    auto resetSimulation = [&]() {
        simulation_running = false;
        goal_scored = false;
        out_of_bounds = false;
        ball_initialized = false;

        // Reset positions
        sprite_character.setPosition(kCharacterInitialX, kGroundLineY);
        sprite_cart.setPosition(kCartInitialX, kGroundLineY);

        // Reset fields
        speed_str = "11.5";
        gravity_str = "9.8";
        speed_text.setString(speed_str);
        gravity_text.setString(gravity_str);

        // Reset angle
        arrow_angle = kDefaultAngleDeg;
        angle_str = "45.0";
        angle_text.setString(angle_str);

        active_field = kNoActiveField;
        };

    //-----------------------------------------------------------------------------
    // Main Loop
    //-----------------------------------------------------------------------------
    while (window.isOpen()) {
        float dt = 1.f / kFrameRateLimit;

        //-------------------------------------------------------------------------
        // Event Handling
        //-------------------------------------------------------------------------
        sf::Event event;
        while (window.pollEvent(event)) {
            // Handle window close
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            // Handle window resizing 
            else if (event.type == sf::Event::Resized) {
                sf::Vector2u new_size(event.size.width, event.size.height);
                sf::Vector2u tex_size = background_texture.getSize();
                float scale_x = static_cast<float>(new_size.x) / tex_size.x;
                float scale_y = static_cast<float>(new_size.y) / tex_size.y;
                sprite_background.setScale(scale_x, scale_y);
            }
            // Mouse Pressed
            else if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mouse_pos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
                if (!simulation_running && !ball_initialized) {
                    // Start simulation if "Simulate" button is clicked
                    if (simulate_button.getGlobalBounds().contains(mouse_pos)) {
                        float initial_speed = ParseFloat(speed_str, kDefaultSpeed);
                        float gravity_val = ParseFloat(gravity_str, kDefaultGravity);

                        // Convert arrow_angle to projectile angle:
                        // projectile angle: 0°=Right, 90°=Up
                        // arrow_angle: 0°=Up, 90°=Right
                        // Relationship: projectile_angle = 90° - arrow_angle
                        float initial_angle = 90.f - arrow_angle;

                        simulation_running = true;

                        float ball_start_x_m = sprite_character.getPosition().x / kScale;
                        float ball_start_y_m = (sprite_character.getPosition().y - 251.f) / kScale;
                        volleyball = Ball(ball_start_x_m, ball_start_y_m, initial_speed, initial_angle, gravity_val, kScale);

                        // Setup ball sprite
                        sf::Sprite sprite_ball(ball_texture);
                        sprite_ball.setScale(0.25f, 0.25f);
                        sf::FloatRect ball_bounds = sprite_ball.getLocalBounds();
                        sprite_ball.setOrigin(ball_bounds.width / 2.f, ball_bounds.height / 2.f);
                        volleyball.setSprite(sprite_ball);

                        ball_initialized = true;
                    }
                    else {
                        // Check if user clicked on character (vertical dragging)
                        if (sprite_character.getGlobalBounds().contains(mouse_pos)) {
                            dragging_character = true;
                            drag_offset = sprite_character.getPosition() - mouse_pos;
                        }
                        // Check if user clicked on cart (horizontal dragging)
                        else if (sprite_cart.getGlobalBounds().contains(mouse_pos)) {
                            dragging_cart = true;
                            drag_offset = sprite_cart.getPosition() - mouse_pos;
                        }
                        // Check speed field activation
                        else if (speed_field_rect.getGlobalBounds().contains(mouse_pos)) {
                            active_field = kSpeedField;
                        }
                        // Check gravity field activation
                        else if (gravity_field_rect.getGlobalBounds().contains(mouse_pos)) {
                            active_field = kGravityField;
                        }
                        else {
                            // Check if user clicked on arrow (angle adjustment)
                            sf::FloatRect arrow_bounds = arrow_shape.getGlobalBounds();
                            if (arrow_bounds.contains(mouse_pos)) {
                                dragging_arrow = true;
                            }
                            else {
                                active_field = kNoActiveField;
                            }
                        }
                    }
                }
                else if (!simulation_running && ball_initialized) {
                    // If simulation ended, allow "Reset"
                    if (reset_button.getGlobalBounds().contains(mouse_pos)) {
                        resetSimulation();
                    }
                }
            }
            // Mouse Released
            else if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    dragging_character = false;
                    dragging_cart = false;
                    dragging_arrow = false;
                }
            }
            // Mouse Moved (dragging to adjust positions/angle)
            else if (event.type == sf::Event::MouseMoved && !simulation_running && !ball_initialized) {
                sf::Vector2f mouse_pos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));

                if (dragging_character) {
                    // Move character vertically only
                    float new_y = mouse_pos.y + drag_offset.y;
                    float max_y = kGroundLineY - (5.35f * kScale); // Limit height dragging so char stays in window 
                    if (new_y < max_y) new_y = max_y;
                    if (new_y > kGroundLineY) new_y = kGroundLineY; // Prevent height dragging down
                    sprite_character.setPosition(sprite_character.getPosition().x, new_y);
                }
                else if (dragging_cart) {
                    // Move cart horizontally only
                    float new_x = mouse_pos.x + drag_offset.x;
                    sprite_cart.setPosition(new_x, sprite_cart.getPosition().y);
                }
                else if (dragging_arrow) {
                    // Recompute angle based on mouse position relative to arrow pivot
                    sf::Vector2f arrow_pivot = sprite_character.getPosition() + character_arrow_offset;
                    sf::Vector2f diff = mouse_pos - arrow_pivot;
                    float angle_rad = std::atan2(diff.y, diff.x);
                    float angle_deg = angle_rad * 180.f / 3.14159f + 90.f;

                    arrow_angle = angle_deg;

                    // Update angle text display
                    std::stringstream angle_stream;
                    angle_stream << std::fixed << std::setprecision(1) << arrow_angle;
                    angle_str = angle_stream.str();
                    angle_text.setString(angle_str);
                }
            }
            // Text Entered (input in speed/gravity fields)
            else if (event.type == sf::Event::TextEntered && !simulation_running && !ball_initialized) {
                if (active_field != kNoActiveField) {
                    uint32_t unicode = event.text.unicode;
                    std::string* target = nullptr;
                    sf::Text* target_text = nullptr;

                    if (active_field == kSpeedField) {
                        target = &speed_str;
                        target_text = &speed_text;
                    }
                    else if (active_field == kGravityField) {
                        target = &gravity_str;
                        target_text = &gravity_text;
                    }

                    if (target && target_text) {
                        if (unicode == '\r') {
                            // Enter pressed - stop editing
                            active_field = kNoActiveField;
                        }
                        else if (unicode == '\b') {
                            // Backspace - remove last character if exists
                            if (!target->empty()) {
                                target->pop_back();
                                target_text->setString(*target);
                            }
                        }
                        else if ((unicode >= '0' && unicode <= '9') || unicode == '.') {
                            // Append digit or decimal point
                            // Allow only one decimal point
                            if (unicode == '.' && target->find('.') != std::string::npos) {
                                // Ignore additional decimal points
                            }
                            else {
                                target->push_back(static_cast<char>(unicode));
                                target_text->setString(*target);
                            }
                        }
                    }
                }
            }
        }

        //-------------------------------------------------------------------------
        // Update Logic (runs every frame)
        //-------------------------------------------------------------------------

        // Update arrow position and rotation
        {
            sf::Vector2f arrow_pivot = sprite_character.getPosition() + character_arrow_offset;
            arrow_shape.setPosition(arrow_pivot);
            arrow_shape.setRotation(arrow_angle);
        }

        // Update distance and height text (difference between character and cart)
        {
            float dist_m = std::fabs(sprite_character.getPosition().x - sprite_cart.getPosition().x) / kScale;
            std::stringstream dist_ss;
            dist_ss << "Distance: " << std::fixed << std::setprecision(2) << dist_m << " m";
            distance_text.setString(dist_ss.str());

            float height_diff_m = std::fabs((sprite_character.getPosition().y - sprite_cart.getPosition().y) / kScale);
            std::stringstream height_ss;
            height_ss << "Height: " << std::fixed << std::setprecision(2) << height_diff_m << " m";
            height_text.setString(height_ss.str());
        }

        // Update platform rectangle under character if character is above ground
        {
            float char_y = sprite_character.getPosition().y;
            float char_x = sprite_character.getPosition().x;
            if (char_y < kGroundLineY) {
                float platform_height = kGroundLineY - char_y;
                platform_rect.setSize(sf::Vector2f(platform_width, platform_height));
                platform_rect.setPosition(char_x - platform_width / 2.f, char_y + 130.f);
            }
            else {
                platform_rect.setSize(sf::Vector2f(0.f, 0.f));
            }
        }

        // Run physics step if simulation is active
        if (simulation_running && ball_initialized) {
            volleyball.update(dt);

            float basket_x_m = sprite_cart.getPosition().x / kScale;
            float basket_y_m = sprite_cart.getPosition().y / kScale;

            // Check if goal scored (ball in basket vicinity)
            if (volleyball.isScored(basket_x_m, basket_y_m, 1.0f)) {
                simulation_running = false;
                goal_scored = true;
            }

            // Check if ball goes out of visible bounds
            if (volleyball.isOutOfBounds(static_cast<float>(window_size.x) / kScale,
                static_cast<float>(window_size.y) / kScale)) {
                simulation_running = false;
                out_of_bounds = true;
            }
        }

        //-------------------------------------------------------------------------
        // Rendering
        //-------------------------------------------------------------------------
        window.clear();
        window.draw(sprite_background);

        // Draw platform when needed
        if (platform_rect.getSize().y > 0.f) {
            window.draw(platform_rect);
        }

        window.draw(sprite_character);
        window.draw(sprite_cart);
        window.draw(distance_text);
        window.draw(height_text);

        // If simulation not started yet, show UI for input and angle arrow
        if (!ball_initialized && !simulation_running) {
            // Draw arrow (angle indicator)
            window.draw(arrow_shape);

            // Draw input fields and labels
            window.draw(speed_field_rect);
            window.draw(angle_field_rect);
            window.draw(gravity_field_rect);
            window.draw(speed_label);
            window.draw(angle_label);
            window.draw(gravity_label);

            // If active field is speed field, show cursor
            if (active_field == kSpeedField) {
                sf::FloatRect speed_bounds = speed_text.getLocalBounds();
                sf::Text cursor("|", font, 20);
                cursor.setFillColor(sf::Color::Black);
                cursor.setPosition(speed_text.getPosition().x + speed_bounds.width + 2.f, speed_text.getPosition().y);
                window.draw(speed_text);
                window.draw(cursor);
            }
            else {
                window.draw(speed_text);
            }

            // Angle is read-only
            window.draw(angle_text);

            // If active field is gravity field, show cursor
            if (active_field == kGravityField) {
                sf::FloatRect grav_bounds = gravity_text.getLocalBounds();
                sf::Text cursor("|", font, 20);
                cursor.setFillColor(sf::Color::Black);
                cursor.setPosition(gravity_text.getPosition().x + grav_bounds.width + 2.f, gravity_text.getPosition().y);
                window.draw(gravity_text);
                window.draw(cursor);
            }
            else {
                window.draw(gravity_text);
            }

            // Draw simulate button
            window.draw(simulate_button);
            window.draw(simulate_text);
        }

        // Draw the ball if initialized
        if (ball_initialized) {
            volleyball.draw(window);
        }

        // If simulation ended, show result and allow reset
        if (!simulation_running && ball_initialized) {
            if (goal_scored) {
                status_text.setString("Goal!");
            }
            else if (out_of_bounds) {
                status_text.setString("No Goal!");
            }
            window.draw(status_text);

            // Draw reset button
            window.draw(reset_button);
            window.draw(reset_text);
        }

        // Present the frame
        window.display();
    }
}
