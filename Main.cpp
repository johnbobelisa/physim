#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

int main() {
    // Create a window with a resolution of 1920x1080 pixels.
    // sf::Style::Close means the window will have a close button but not be resizable.
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Physics Simulations Dashboard", sf::Style::Close);

    // Limit the frame rate to 60 frames per second.
    // This helps keep the animation smooth and reduces CPU/GPU usage.
    window.setFramerateLimit(60);

    // Load a font from a file to use for drawing text.
    sf::Font font;
    if (!font.loadFromFile("retrogaming.ttf")) {
        // If the font fails to load, we return -1, meaning the program ends here.
        return -1;
    }

    // We define a list of menu items to display on the screen.
    std::vector<std::string> menuItems = {
        "Simulate",
        "Quit"
    };

    // Define colors for the background and text:
    // bgColor: The background of the window.
    // textColor: The default color of non-selected menu items.
    // highlightColor: The color of the currently selected menu item.
    sf::Color bgColor = sf::Color::Black;
    sf::Color textColor = sf::Color::White;
    sf::Color highlightColor = sf::Color::Green;

    // We'll create a vector of sf::Text objects, each representing one menu item.
    std::vector<sf::Text> textObjects;

    // Loop through each string in menuItems and create a corresponding sf::Text object.
    for (const auto& item : menuItems) {
        sf::Text text;
        text.setFont(font);                // Set the font for the text.
        text.setString(item);              // Set the displayed string.
        text.setFillColor(textColor);      // Set the text color for now to the default color.
        text.setCharacterSize(40);         // Set the size of the characters in pixels.
        textObjects.push_back(text);       // Add the text object to our vector.
    }

    // window.getSize().y / 2.0f is the vertical center of the window
    // (menuItems.size() * 50.0f / 2.0f) is "number of items in the menu * spacing between text"
    // and dividing by 2 gives us the distance from the center of the menu to the topmost item.
    float startY = (window.getSize().y / 2.0f) - (menuItems.size() * 50.0f / 2.0f);

    // Position each text object individually.
    for (size_t i = 0; i < textObjects.size(); ++i) {
        // Get the width of the current text to center it horizontally.
        float textWidth = textObjects[i].getLocalBounds().width;

        // xPos: we place the text so that its center is at the window's center.
        float xPos = (window.getSize().x / 2.0f) - (textWidth / 2.0f);

        // yPos: starting at 'startY', each subsequent line is placed 50 pixels lower.
        float yPos = startY + i * 50.0f;

        // Set the position of the text on the screen.
        textObjects[i].setPosition(xPos, yPos);
    }

    // We'll keep track of which menu item is currently selected.
    int selectedItem = 0;

    // Highlight the initially selected menu item by changing its color to highlightColor.
    textObjects[selectedItem].setFillColor(highlightColor);

    // ----------------------
    // Creating a bounding rectangle around the entire menu:
    //
    // We want a rectangle that encapsulates all the menu items, with a certain padding:
    // - 25 pixels extra above and below the tallest text region
    // - 50 pixels extra to the left and right of the widest text region
    //
    // Steps:
    // 1. Find the minimum and maximum X and Y coordinates spanned by the text objects.
    // 2. Add padding (25 pixels in Y direction, 50 pixels in X direction).
    // 3. Create a rectangle shape with these dimensions.
    // ----------------------

    // Initialize min and max values. We'll update these as we inspect the text objects.
    // We start min values high so that any text will lower them, and max values low so that any text will raise them.
    float minX = 9999999.f;
    float maxX = -9999999.f;
    float minY = 9999999.f;
    float maxY = -9999999.f;

    // Loop through all text items to find their bounding boxes.
    for (auto& t : textObjects) {
        // getLocalBounds() gives the text's size relative to its position.
        // We need the absolute positions on the screen, so we add the position of the text.
        sf::FloatRect bounds = t.getLocalBounds();

        // The left and top from getLocalBounds() are usually 0,0 but can sometimes differ due to glyph metrics.
        // The actual positioned coordinates of the text on screen:
        float left = t.getPosition().x;
        float top = t.getPosition().y;
        float right = left + bounds.width;      // right edge of the text
        float bottom = top + bounds.height;     // bottom edge of the text

        // Update min and max values
        if (left < minX)  minX = left;
        if (right > maxX) maxX = right;
        if (top < minY)   minY = top;
        if (bottom > maxY)maxY = bottom;
    }

    // Now we have the minimum and maximum extents of all text.
    // Apply the desired padding:
    //  - 50 pixels extra on the left and right
    //  - 25 pixels extra on the top and bottom
    float paddingX = 50.f;  // horizontal padding
    float paddingY = 25.f;  // vertical padding

    // Calculate the width and height of the final bounding rectangle.
    float menuWidth = (maxX - minX) + (paddingX * 2.f);
    float menuHeight = (maxY - minY) + (paddingY * 2.f);

    // Create a rectangle to outline the entire menu.
    sf::RectangleShape menuBox(sf::Vector2f(menuWidth, menuHeight));

    // Set the position of the menuBox so that it covers from minX - paddingX to maxX + paddingX,
    // and similarly for Y:
    menuBox.setPosition(minX - paddingX, minY - paddingY);

    // Set the menuBox appearance: transparent fill, white outline.
    menuBox.setFillColor(sf::Color::Transparent);
    menuBox.setOutlineColor(highlightColor);
    menuBox.setOutlineThickness(2.f);

    // Main event and drawing loop.
    // This loop runs until we close the window.
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            // If the user clicks the close button on the window, we close the application.
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Check for keyboard input events.
            if (event.type == sf::Event::KeyPressed) {
                // If the user presses the Up arrow key, move the selection upwards in the menu.
                if (event.key.code == sf::Keyboard::Up) {
                    // First, restore the currently selected item to the default text color.
                    textObjects[selectedItem].setFillColor(textColor);
                    // Move selection up: we use modular arithmetic to wrap around.
                    selectedItem = (selectedItem - 1 + (int)menuItems.size()) % (int)menuItems.size();
                    // Highlight the new selected item.
                    textObjects[selectedItem].setFillColor(highlightColor);
                }
                // If the user presses the Down arrow key, move the selection downwards.
                else if (event.key.code == sf::Keyboard::Down) {
                    // Restore current selected item color.
                    textObjects[selectedItem].setFillColor(textColor);
                    // Move selection down.
                    selectedItem = (selectedItem + 1) % (int)menuItems.size();
                    // Highlight the newly selected item.
                    textObjects[selectedItem].setFillColor(highlightColor);
                }
                // If the user presses the Enter key, we "activate" the selected menu item.
                else if (event.key.code == sf::Keyboard::Enter) {
                    // Find which menu item is selected.
                    std::string chosen = menuItems[selectedItem];

                    // If user chooses "Quit", close the window, ending the program.
                    if (chosen == "Quit") {
                        window.close();
                    }
                    // If user chooses "Simulate", you can then run the chosen simulation.
                    else if (chosen == "Simulate") {
                        // Create menu options for all available topics (ex. Motion in One Dimension)
                        // Clicking any of the available topics will bring us to its sub-topics (ex. Freely Falling Objects)
                    }
                }
            }
        }

        // Clear the window with the background color.
        window.clear(bgColor);

        // Draw the bounding box around the menu. This box highlights all the menu items together.
        window.draw(menuBox);

        // Draw each of the menu items.
        for (auto& t : textObjects) {
            window.draw(t);
        }

        // Display everything we’ve drawn on the screen.
        window.display();
    }

    return 0; // End of the program.
}
