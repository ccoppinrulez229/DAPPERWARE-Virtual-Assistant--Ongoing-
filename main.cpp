#include <iostream>
#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <map>
#include <string>
#include <sapi.h>
#include <thread>
#include <locale>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <functional>
#include <stdlib.h>


using namespace std;

bool talking = false;
bool dramatic_pause = false;
bool debug = false;
vector<string> apps_open;

class dapperware {
    sf::Sprite dapperware_sprite;
    map<string,sf::Texture> texture_map;//allocates the texture map to memory
    int anim_frame = 0;
    float frame_time = 0.05; //framerate of dapperware's animations
    sf::Clock clock;
    bool animation_active = true;

public:
    dapperware() { //constructor. initializes the texture map for all of dapperware's animations
        dapperware_sprite.setPosition(100,285);
        dapperware_sprite.setScale(0.96f,0.96f);
        sf::Texture dapper_texture;
        dapper_texture.loadFromFile("files/images/animations/spin.png");
        texture_map["spin"]= dapper_texture;
        dapperware_sprite.setTexture(texture_map["spin"]);

    }
    void UpdateSprite() { //will update the sprite to the current animation frame using rect. called every frame
        if (clock.getElapsedTime().asSeconds() >= frame_time && animation_active) {
            anim_frame++;
            if (450*anim_frame < dapperware_sprite.getTexture()->getSize().x) {
                dapperware_sprite.setTextureRect(sf::IntRect(450*anim_frame,0,450,360));
            }
            else {
                anim_frame=0;
            }
            clock.restart();
        }
    }

    sf::Sprite& getSprite() {
        return this->dapperware_sprite;
    }
};

class context_menu_button {
    sf::RectangleShape rectangle;
    sf::Text text;
    int rectangle_y=237;
    int text_y=239;
    string nam;
public:
    context_menu_button(string name,sf::Font& font, int rect_y, int tex_y) { //constructor
        rectangle.setSize(sf::Vector2f(135,20));
        rectangle.setOutlineThickness(1);
        rectangle.setOutlineColor(sf::Color(65,87,104,255));
        rectangle.setFillColor(sf::Color(153,167,179,255));
        rectangle.setPosition(100,rect_y);

        text.setFont(font);
        text.setString(name);
        text.setCharacterSize(12);
        text.setFillColor(sf::Color::Black);
        text.setPosition(105,tex_y);
        nam = name;

    }
    sf::RectangleShape returnShape() {
        return rectangle;
    }
    void draw(sf::RenderWindow& window) {
        window.draw(rectangle);
        window.draw(text);
    }
    string click() {
        return nam;
    }
    void hovered() {
        rectangle.setFillColor(sf::Color(130,143,154,255));
    }
    void not_hovered() {
        rectangle.setFillColor(sf::Color(153,167,179,255));
    }
};

class speech_bubble {
    sf::Sprite bottom_sprite;
    sf::Sprite middle_sprite;
    sf::Sprite top_sprite;
    map<string,sf::Texture> texture_map;
    string text;
    sf::Text str;
public:
    speech_bubble(string tex,sf::Font& font) {
        bottom_sprite.setPosition(215,293);
        bottom_sprite.setScale(0.35f,0.35f);
        middle_sprite.setScale(0.35f,0.65f);
        top_sprite.setScale(0.35f,0.35f);
        text = tex;

        sf::Texture bubble_texture;
        bubble_texture.loadFromFile("files/images/speech_bubble_top.png");
        texture_map["top"]=bubble_texture;
        top_sprite.setTexture(texture_map["top"]);


        bubble_texture.loadFromFile("files/images/speech_bubble_middle.png");
        texture_map["middle"]=bubble_texture;
        middle_sprite.setTexture(texture_map["middle"]);


        bubble_texture.loadFromFile("files/images/speech_bubble_bottom.png");
        texture_map["bottom"]=bubble_texture;
        bottom_sprite.setTexture(texture_map["bottom"]);

        str.setFont(font);
        str.setCharacterSize(12);
        str.setString(tex);
        str.setPosition(223,228);
        str.setFillColor(sf::Color::Black);
    }
    void draw(sf::RenderWindow& window) {
        float middle_sprite_yscale= 0.65f;
        int middle_sprite_yposition=234;
        int top_sprite_yposition=221;
        int str_yposition=228;
        size_t newline_count = std::count(text.begin(), text.end(), '\n');
        if (newline_count > 4) {
            while (newline_count>4) {
                middle_sprite_yscale += 0.75f;
                middle_sprite_yposition-=65;
                top_sprite_yposition-=65;
                str_yposition-=65;
                newline_count -= 4;
            }
        }

        window.draw(bottom_sprite);
        middle_sprite.setScale(0.35f,middle_sprite_yscale);
        middle_sprite.setPosition(215,middle_sprite_yposition);
        window.draw(middle_sprite);
        top_sprite.setPosition(215,top_sprite_yposition);
        str.setPosition(223,str_yposition);
        window.draw(top_sprite);
        window.draw(str);
    }
    void changeStr(string st) {
        str.setString(st);
        text = st;
    }


};



void makeWindowOnTop(sf::RenderWindow& window)
{
    HWND hwnd = window.getSystemHandle();
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

void DebugMenu(bool& debug_visible, bool& talking) {
    sf::Font font;
    font.loadFromFile("files/font.ttf");
    sf::Text debug_text;
    debug_text.setFont(font);
    debug_text.setCharacterSize(15);
    debug_text.setColor(sf::Color::White);
    string txt = "";

    int screenwidth = GetSystemMetrics(SM_CXSCREEN);
    int screenheight = GetSystemMetrics(SM_CYSCREEN);
    sf::RenderWindow debug_window(sf::VideoMode(150,500),"Debug",sf::Style::None);
    debug_window.setPosition(sf::Vector2i(0,0));

    while (debug_window.isOpen()) {
        sf::Event event;
        while (debug_window.pollEvent(event)) {
            auto click = event.mouseButton;
            // Close event
            if (event.type == sf::Event::EventType::Closed) {
                debug_window.close();
                break;
            }
        }

        txt = "Debug Menu\n-------\n";
        if (talking) {txt += "Talking: ON\n";}
        else {txt += "Talking: OFF\n";}
        debug_text.setString(txt);

        debug_window.clear();
        debug_window.draw(debug_text);

        debug_window.display();

        if (!debug_visible) {
            debug_window.setVisible(false);
        }
        else {
            debug_window.setVisible(true);
            debug_window.requestFocus();
        }

        if (debug_window.hasFocus()) {
            makeWindowOnTop(debug_window);
        }

    }
}

void makeWindowTransparent(sf::RenderWindow& window) {
    //sets window transparency
    HWND hwnd = window.getSystemHandle();
    LONG style = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED);  // Add the layered window style
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);  // Set black as transparent
}

void DrawContextMenuButtons(vector<context_menu_button> vect, sf::RenderWindow& window) {
    for (int i=0 ; i<vect.size() ; i++) {
        vect[i].draw(window);
    }
}

std::wstring stringToWString(const std::string& str) {
    wstringstream wss;
    wss << str.c_str();
    return wss.str();

}

vector<string> ReadFileToVec(ifstream& file) {
    string line;
    vector<string> vec;
    while (getline(file,line)) {
        vec.push_back(line);
    }
    return vec;
}

void WriteVecToFile(vector<string>& vec,string filename) {
    ofstream save;
    save.open(filename);
    for (int i=0 ; i<vec.size() ; i++) {
        save << vec[i] << endl;
    }
}

ifstream save("files/save.txt");
vector<string> save_vector = ReadFileToVec(save);
ifstream regsave("files/prodappssave.txt");
vector<string> registered_apps_save = ReadFileToVec(regsave);

string EnterNameWindow() {
    sf::Texture window_texture;
    window_texture.loadFromFile("files/images/empty_window.png");
    sf::Sprite window_sprite;
    window_sprite.setTexture(window_texture);

    sf::Font font;
    font.loadFromFile("files/fixedsys.ttf");

    sf::Text enter_name_text;
    enter_name_text.setFont(font);
    enter_name_text.setString("What is your name?");
    enter_name_text.setCharacterSize(15);
    enter_name_text.setFillColor(sf::Color::White);
    enter_name_text.setPosition(15,32);

    sf::Text user_typed_name;
    user_typed_name.setFont(font);
    user_typed_name.setString("");
    user_typed_name.setCharacterSize(15);
    user_typed_name.setFillColor(sf::Color::White);
    user_typed_name.setPosition(15,55);

    string user_input_string;
    user_typed_name.setString(user_input_string+"_");
    int user_input_string_length=0;

    sf::RenderWindow namewindow(sf::VideoMode(470,263),"Enter Name",sf::Style::None);
    while (namewindow.isOpen()) {

        // Event loop
        sf::Event event;
        while (namewindow.pollEvent(event)) {
            // Close event
            if (event.type == sf::Event::EventType::Closed) {
                namewindow.close();
                break;
            }

            //check for keyboard input to type characters for the name
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode < 128) {
                    string current_char; //initializes a string for the current character typed in
                    current_char = current_char + static_cast<char>(event.text.unicode);

                    if (isalpha(current_char[0])) { //checks if the key pressed is a valid letter
                        if (user_input_string_length<40) {
                            if (user_input_string_length==0) {
                                user_input_string += std::toupper(current_char[0]);
                                user_typed_name.setString(user_input_string+"_");
                                user_input_string_length+=1;
                            }
                            else {
                                user_input_string += std::tolower(current_char[0]);
                                user_typed_name.setString(user_input_string+"_");
                                user_input_string_length+=1;
                            }
                        }
                    }
                }
            }

            if (event.type == sf::Event::KeyPressed) { //will check keyboard inputs
                if (event.key.code == sf::Keyboard::Backspace) { //when backspace is pressed, remove the last character from string if something is there.
                    if (user_input_string_length > 0) {
                        user_input_string.pop_back(); //removes last character
                        user_typed_name.setString(user_input_string+"_");
                        user_input_string_length-=1;
                    }
                }

                if (event.key.code == sf::Keyboard::Enter) {
                    if (user_input_string_length>=1) {
                        namewindow.close();
                        return user_input_string;
                    }
                }
            }
        }

        if (user_input_string_length==40) {
            user_typed_name.setString(user_input_string);
        }
        namewindow.clear(sf::Color::Blue);
        namewindow.draw(window_sprite);
        namewindow.draw(enter_name_text);
        namewindow.draw(user_typed_name);
        namewindow.display();
    }
}

void StickyNote(sf::Font& font, string filename,bool& sticky_notes_close, int& max_notes, int& num_of_notes, bool& visbility) {
    string line;
    int window_width;
    int window_height;
    int window_xpos;
    int window_ypos;
    int color;
    string text;
    ifstream file(filename);
    getline(file,line);
    window_width=stoi(line);
    getline(file,line);
    window_height=stoi(line);
    getline(file,line);
    window_xpos=stoi(line);
    getline(file,line);
    window_ypos=stoi(line);

    sf::Font fonty;
    fonty.loadFromFile("files/font.ttf");
    sf::Texture note_texture;
    note_texture.loadFromFile("files/images/sticky_note.png");
    sf::Sprite note_sprite;
    note_sprite.setTexture(note_texture);

    sf::Text user_typed_name;
    user_typed_name.setFont(fonty);
    user_typed_name.setString("");
    user_typed_name.setCharacterSize(14);
    user_typed_name.setFillColor(sf::Color::Black);
    user_typed_name.setPosition(5,5);

    getline(file,line);
    int user_input_string_length=stoi(line);
    getline(file,line);
    color=stoi(line);

    if (user_input_string_length==0) {
        ofstream reset_note(filename);
        reset_note << window_width << "\n" << window_height << "\n" << window_xpos << "\n" << window_ypos << "\n" << user_input_string_length << "\n" << color << endl;

    }
    string user_input_string = "";
    while (getline(file,line) && user_input_string_length>0) {
        user_input_string += line+"\n";
    }
    if (user_input_string_length>0) {
        user_input_string.pop_back();
    }
    user_typed_name.setString(user_input_string);

    bool text_changed=false;
    bool grabbed = false;
    sf::RenderWindow note_window(sf::VideoMode(window_width,window_height),"note",sf::Style::None);
    note_window.setPosition(sf::Vector2i(window_xpos,window_ypos));
    while (note_window.isOpen()) {
        sf::Vector2i mouseposition = sf::Mouse::getPosition();
        sf::Vector2u windowsize = note_window.getSize();
        sf::Event event;
        auto click = event.mouseButton;
        while (note_window.pollEvent(event)) {
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode < 128 && event.text.unicode != '\b') {
                    string current_char; //initializes a string for the current character typed in
                    current_char = current_char + static_cast<char>(event.text.unicode);
                    user_input_string += current_char[0];
                    user_input_string_length+=1;
                    text_changed = true;
                }
            }

            if (event.type == sf::Event::KeyPressed) { //will check keyboard inputs
                if (event.key.code == sf::Keyboard::Backspace) { //when backspace is pressed, remove the last character from string if something is there.
                    if (user_input_string_length > 0) {
                        user_input_string.pop_back(); //removes last character
                        user_input_string_length -= 1;
                        text_changed = true;
                    }
                }

                if (event.key.code == sf::Keyboard::Enter) {
                    user_input_string+="\n";
                    user_input_string_length+=1;
                    text_changed = true;
                }
            }
            if (click.button==sf::Mouse::Left) {
                if (event.type == sf::Event::EventType::MouseButtonPressed) {
                    if (note_sprite.getGlobalBounds().contains(click.x,click.y)) {
                        grabbed = true;
                        if (sticky_notes_close) {
                            file.close();
                            remove(filename.c_str());
                            max_notes--;
                            num_of_notes--;
                            note_window.close();
                            break;
                        }
                    }
                }
            }
            if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                if (event.type == sf::Event::EventType::MouseButtonPressed) {
                    if (note_sprite.getGlobalBounds().contains(click.x,click.y)) {
                        color++;
                        if (color>6) {
                            color = 1;
                        }
                        text_changed = true;
                    }
                }
            }

            if (event.type == sf::Event::EventType::MouseButtonReleased) {
                grabbed = false;
            }
        }
        if (grabbed) {
            note_window.setPosition(sf::Vector2i(mouseposition.x-110,mouseposition.y-100));
        }
        if (color==1) {
            note_sprite.setColor(sf::Color::White);
        }
        else if (color==2) {
            note_sprite.setColor(sf::Color::Blue);
        }
        else if (color==3) {
            note_sprite.setColor(sf::Color::Cyan);
        }
        else if (color==4) {
            note_sprite.setColor(sf::Color::Green);
        }
        else if (color==5) {
            note_sprite.setColor(sf::Color::Magenta);
        }
        else if (color==6) {
            note_sprite.setColor(sf::Color::Red);
        }
        window_xpos = note_window.getPosition().x;
        window_ypos = note_window.getPosition().y;
        user_typed_name.setString(user_input_string);

        note_window.clear();
        note_window.draw(note_sprite);
        note_window.draw(user_typed_name);
        if (text_changed) {
            ofstream save_note(filename);
            file.seekg(std::ios::beg);
            string note_text = "";
            getline(file,line);
            getline(file,line);
            getline(file,line);
            getline(file,line);
            for (int i=0 ; i<user_input_string.length() ; i++) {
                note_text+=user_input_string[i];
            }
            save_note << window_width << "\n" << window_height << "\n" << window_xpos << "\n" << window_ypos << "\n" << user_input_string_length << "\n" << color << "\n" << note_text << endl;
            text_changed = false;
        }
        note_window.display();
        if (note_window.hasFocus()) {
            makeWindowOnTop(note_window);
        }
        if (visbility) {
            note_window.setVisible(true);
        }
        else {
            note_window.setVisible(false);
        }
    }
}

void StickyNoteManager(int screenwidth, int screenheight, bool& sticky_notes_open, sf::Font& font) {
    map<string,sf::Texture> texture_map;
    texture_map["add"].loadFromFile("files/images/sticky_note_add.png");
    texture_map["visibility_on"].loadFromFile("files/images/sticky_note_visibility_on.png");
    texture_map["visibility_off"].loadFromFile("files/images/sticky_note_visibility_off.png");
    texture_map["delete_on"].loadFromFile("files/images/sticky_note_delete_on.png");
    texture_map["delete_off"].loadFromFile("files/images/sticky_note_delete_off.png");
    sf::Sprite add_sprite;
    add_sprite.setTexture(texture_map["add"]);
    add_sprite.setScale(1.8f,1.8f);
    add_sprite.setPosition(65,0);
    sf::Sprite visibility_sprite;
    visibility_sprite.setTexture(texture_map["visibility_on"]);
    visibility_sprite.setScale(1.8f,1.8f);
    visibility_sprite.setPosition(65,55);
    sf::Sprite delete_sprite;
    delete_sprite.setTexture(texture_map["delete_off"]);
    delete_sprite.setScale(1.8f,1.8f);
    delete_sprite.setPosition(65,110);
    bool visibility = true;
    bool creating_notes = false;
    bool first_load = true;
    bool delete_on = false;
    sf::RenderWindow manager(sf::VideoMode(300,179),"manager",sf::Style::None);
    makeWindowTransparent(manager);
    manager.setPosition(sf::Vector2i(screenwidth-126,screenheight-800));
    int max_notes = 1;
    while (manager.isOpen()) {
        sf::Event event;
        auto click = event.mouseButton;
        while (manager.pollEvent(event)) {
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                //cout << "clicked" << endl;
                if (add_sprite.getGlobalBounds().contains(click.x,click.y)) {
                    namespace fs = std::filesystem;
                    fs::path directoryPath = "files/sticky notes"; // Replace with the actual path
                    int num_of_notes = 0;
                    for (const auto& entry : fs::directory_iterator(directoryPath)) {
                        //std::cout << entry.path().filename() << std::endl;
                        num_of_notes++;
                    }
                    while (true) { //checks to see if the note added will have a filename of an already existing note.
                        if (fs::exists("files/sticky notes/stickynote"+to_string(num_of_notes)+".txt")) {
                            num_of_notes++;
                        }
                        if (!fs::exists("files/sticky notes/stickynote"+to_string(num_of_notes)+".txt")) {
                            break;
                        }
                    }
                    ofstream new_note("files/sticky notes/stickynote"+to_string(num_of_notes)+".txt");
                    new_note << 245 << "\n" << 245 << "\n" << 0 << "\n" << 0 << "\n" << 0 << "\n" << 1 << endl;
                    std::thread note(StickyNote,std::ref(font),"files/sticky notes/stickynote"+to_string(num_of_notes)+".txt",std::ref(delete_on),std::ref(max_notes),std::ref(num_of_notes),std::ref(visibility));
                    note.detach();
                    cout << "added note" << endl;
                }
                if (delete_sprite.getGlobalBounds().contains(click.x,click.y)) {
                    delete_on = !delete_on;
                }
                if (visibility_sprite.getGlobalBounds().contains(click.x,click.y)) {
                    visibility = !visibility;
                }
            }
        }

        namespace fs = std::filesystem;
        fs::path directoryPath = "files/sticky notes"; // Replace with the actual path


        int num_of_notes = 0;
        for (const auto& entry : fs::directory_iterator(directoryPath)) {
            //std::cout << entry.path().filename() << std::endl;
            num_of_notes++;
        }

        if (max_notes<num_of_notes) {
            max_notes = num_of_notes;
            //sticky_notes_close = true;
            creating_notes = false;
        }
        if (!creating_notes) { //opens all sticky notes when the manager is opened for the first time
            creating_notes = true;
            int intended_dir=0;
            for (const auto& entry : fs::directory_iterator(directoryPath)) {
                if (first_load) {
                    string filepath = "files/sticky notes/" + entry.path().filename().generic_string();
                    std::thread note(StickyNote,std::ref(font),filepath,std::ref(delete_on),std::ref(max_notes),std::ref(num_of_notes),std::ref(visibility));
                    note.detach();
                }
                intended_dir++;
            }
            first_load = false;
        }
        if (delete_on) {
            delete_sprite.setTexture(texture_map["delete_on"]);
        }
        else {
            delete_sprite.setTexture(texture_map["delete_off"]);
        }
        if (visibility) {
            visibility_sprite.setTexture(texture_map["visibility_on"]);
        }
        else {
            visibility_sprite.setTexture(texture_map["visibility_off"]);
        }

        manager.clear(sf::Color(0,0,0,1));
        manager.draw(add_sprite);
        manager.draw(visibility_sprite);
        manager.draw(delete_sprite);
        manager.display();
        if (manager.hasFocus()) {
            makeWindowOnTop(manager);
        }
    }
}

static BOOL CALLBACK GetActiveAppWindows(HWND hWnd, LPARAM lparam) {
    int length = GetWindowTextLength(hWnd);
    char* buffer = new char[length + 1];
    GetWindowText(hWnd, buffer, length + 1);
    std::string windowTitle(buffer);
    delete[] buffer;

    // List visible windows with a non-empty title
    if (IsWindowVisible(hWnd) && length != 0) {
        //cout << windowTitle << endl;
        reverse(windowTitle.begin(),windowTitle.end());
        string temp="";
        for (int i=0 ; i<windowTitle.size(); i++) {
            if (windowTitle[i]!='-') {
                temp+=windowTitle[i];
            }
            else {
                break;
            }
        }
        if (temp[temp.size()-1]==' ') {
            temp.pop_back();
        }
        reverse(temp.begin(),temp.end());

        if (find(apps_open.begin(),apps_open.end(),temp)==apps_open.end() && find(registered_apps_save.begin(),registered_apps_save.end(),temp)==registered_apps_save.end()) {
            apps_open.push_back(temp); //will only add an app to available vector if they are not currently registered and are not already in the apps open vector
        }
    }
    return TRUE;

} //got this from chat

void FindTheActiveWindow() { //find the currently focused window and checks if it's part of the registered apps

    HWND foregroundWindow = GetForegroundWindow();

    /*if (foregroundWindow == nullptr) {
        std::cerr << "Error: Could not get the foreground window." << std::endl;
    }*/

    DWORD processId;
    GetWindowThreadProcessId(foregroundWindow, &processId);

    /*if (processId == 0) {
        std::cerr << "Error: Could not get process ID." << std::endl;
    }*/

    // Optional: Get the window title
    const int bufferSize = 256;
    wchar_t windowTitle[bufferSize];
    if (foregroundWindow!=nullptr && processId!=0) {
        GetWindowTextW(foregroundWindow, windowTitle, bufferSize);
    }
    else {
        //return false;
    }
    string window_title(std::begin(windowTitle), std::end(windowTitle));
    //cout << window_title << endl;
    for (int i=0 ; i<registered_apps_save.size() ; i++) {
        if (window_title.find(registered_apps_save[i])!=string::npos) {
            cout << "New active registered app found: " << registered_apps_save[i] << endl;
            //return true;
        }
    }

    //std::wcout << "Window title: " << windowTitle << std::endl;
}

void ProductivityAppsWindow() {
    sf::Font font;
    font.loadFromFile("files/font.ttf");
    sf::Texture right;
    right.loadFromFile("files/images/right_arrow.png");
    sf::Sprite right_button;
    right_button.setTexture(right);
    right_button.setPosition(297,190);
    sf::Texture refresh;
    refresh.loadFromFile("files/images/refresh.png");
    sf::Sprite refresh_button;
    refresh_button.setTexture(refresh);
    refresh_button.setScale(0.5f,0.5f);
    refresh_button.setPosition(9,71);
    sf::Texture delete_tex;
    delete_tex.loadFromFile("files/images/delete_button.png");
    sf::Sprite delete_sprite;
    delete_sprite.setTexture(delete_tex);
    delete_sprite.setScale(0.5f,0.5f);
    delete_sprite.setPosition(613,71);

    string selected_app = "";
    map<string,sf::Text> app_text_map;
    map<string,sf::Text> registered_app_text_map;
    sf::Texture window_tex;
    window_tex.loadFromFile("files/images/prod_apps_window.png");
    sf::Sprite window_sprite;
    window_sprite.setTexture(window_tex);

    sf::RenderWindow prod_window(sf::VideoMode(650,450),"prod",sf::Style::None);
    while (prod_window.isOpen()) {
        sf::Event event;
        auto click = event.mouseButton;
        while (prod_window.pollEvent(event)) {
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                //cout << "clicked" << endl;
                if (refresh_button.getGlobalBounds().contains(click.x,click.y)) {
                    app_text_map.clear();
                    apps_open.clear();
                    EnumWindows(GetActiveAppWindows, NULL); //EnumWindows iterates through all top-level windows on screen, placing all open apps in vector
                    int starting_y_pos = 75;
                    for (int i=0 ; i<apps_open.size() ; i++) {
                        app_text_map[apps_open[i]].setFont(font);
                        app_text_map[apps_open[i]].setString(apps_open[i]);
                        app_text_map[apps_open[i]].setColor(sf::Color::Black);
                        app_text_map[apps_open[i]].setCharacterSize(12);
                        app_text_map[apps_open[i]].setPosition(45,starting_y_pos);
                        starting_y_pos+=15;
                    }
                }
                if (right_button.getGlobalBounds().contains(click.x,click.y) && selected_app!="") {
                    registered_apps_save.push_back(selected_app);
                    WriteVecToFile(registered_apps_save,"files/prodappssave.txt");
                    app_text_map.clear();
                    apps_open.clear();
                    EnumWindows(GetActiveAppWindows, NULL); //EnumWindows iterates through all top-level windows on screen
                    int starting_y_pos = 75;
                    for (int i=0 ; i<apps_open.size() ; i++) {
                        app_text_map[apps_open[i]].setFont(font);
                        app_text_map[apps_open[i]].setString(apps_open[i]);
                        app_text_map[apps_open[i]].setColor(sf::Color::Black);
                        app_text_map[apps_open[i]].setCharacterSize(12);
                        app_text_map[apps_open[i]].setPosition(45,starting_y_pos);
                        starting_y_pos+=15;
                    }
                    selected_app = "";
                }
                for (auto it=app_text_map.begin() ; it!=app_text_map.end();it++) {
                    if (it->second.getGlobalBounds().contains(click.x,click.y)) {
                        for (auto bruh=app_text_map.begin() ; bruh!=app_text_map.end() ; bruh++) {
                            bruh->second.setStyle(sf::Text::Regular);
                        }
                        it->second.setStyle(sf::Text::Bold);
                        selected_app = it->first;
                    }
                }
                for (auto it=registered_app_text_map.begin() ; it!=registered_app_text_map.end();it++) {
                    if (it->second.getGlobalBounds().contains(click.x,click.y)) {
                        //cout << "click" << endl;
                        for (auto hyoh=registered_app_text_map.begin() ; hyoh!=registered_app_text_map.end() ; hyoh++) {
                            hyoh->second.setStyle(sf::Text::Regular);
                        }
                        it->second.setStyle(sf::Text::Bold);
                        cout << "bold" << endl;
                        selected_app = it->first;
                        //cout << selected_app << endl;
                        for (auto br=app_text_map.begin() ; br != app_text_map.end() ; br++) {
                            br->second.setStyle(sf::Text::Regular);
                        }
                    }
                }

                if (delete_sprite.getGlobalBounds().contains(click.x,click.y) && selected_app!="") {
                    registered_apps_save.erase(remove(registered_apps_save.begin(),registered_apps_save.end(),selected_app),registered_apps_save.end()); //removes the app from the vector
                    WriteVecToFile(registered_apps_save,"files/prodappssave.txt");
                    app_text_map.clear();
                    apps_open.clear();
                    EnumWindows(GetActiveAppWindows, NULL); //EnumWindows iterates through all top-level windows on screen
                    int starting_y_pos = 75;
                    for (int i=0 ; i<apps_open.size() ; i++) {
                        app_text_map[apps_open[i]].setFont(font);
                        app_text_map[apps_open[i]].setString(apps_open[i]);
                        app_text_map[apps_open[i]].setColor(sf::Color::Black);
                        app_text_map[apps_open[i]].setCharacterSize(12);
                        app_text_map[apps_open[i]].setPosition(45,starting_y_pos);
                        starting_y_pos+=15;
                    }
                }
            }
        }

        int starting_y_pos = 75;
        registered_app_text_map.clear();
        for (int i=0 ; i<registered_apps_save.size() ; i++) {
            registered_app_text_map[registered_apps_save[i]].setFont(font);
            registered_app_text_map[registered_apps_save[i]].setString(registered_apps_save[i]);
            registered_app_text_map[registered_apps_save[i]].setColor(sf::Color::Black);
            registered_app_text_map[registered_apps_save[i]].setCharacterSize(12);
            registered_app_text_map[registered_apps_save[i]].setPosition(370,starting_y_pos);
            starting_y_pos+=15;
        }

        prod_window.clear(sf::Color::White);
        for (auto it = app_text_map.begin() ; it!=app_text_map.end() ; it++) {
            prod_window.draw(it->second);
        }
        for (auto hya = registered_app_text_map.begin() ; hya!=registered_app_text_map.end() ; hya++) {
            prod_window.draw(hya->second);
        }

        prod_window.draw(window_sprite);
        prod_window.draw(refresh_button);
        prod_window.draw(right_button);
        prod_window.draw(delete_sprite);
        prod_window.display();
    }
}


void textToSpeech(speech_bubble& bubble, ifstream& dialogue, string& current_dialog_file) {
    string line;
    HRESULT hr = ::CoInitialize(NULL);

    ISpVoice* pVoice = NULL;
    hr = ::CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void**)&pVoice);
    while (true) {
        if (talking) {
            //cout << "talk tuah" << endl;
            //talking = false;

            string templine = "";
            while (getline(dialogue,line)) { //iterates through all dialogue in the string vector
                if (line=="//") { //checks for spacings and halts further dialogue based on certain dialogue files
                    dramatic_pause = true; //set to true so the dialogue box can be hidden while halted
                    if (current_dialog_file=="intro" && save_vector[1]=="IntroNotDone") {
                        save_vector[0]= EnterNameWindow();
                        WriteVecToFile(save_vector,"files/save.txt");
                        dramatic_pause = false;
                    }
                    continue;
                }
                if (line=="---") { //when the end of the dialog file is reached
                    if (current_dialog_file=="intro" && save_vector[1]=="IntroNotDone") {
                        current_dialog_file.clear();
                        save_vector[1]="IntroDone";
                        WriteVecToFile(save_vector,"files/save.txt");
                    }
                    if (current_dialog_file=="sticky notes intro" && save_vector[2]=="StickyNotesIntroNotDone") {
                        current_dialog_file.clear();
                        save_vector[2]= "StickyNotesIntroDone";
                        WriteVecToFile(save_vector,"files/save.txt");
                    }
                    if (current_dialog_file=="register apps intro" && save_vector[3]=="ProdAppsIntroNotDone") {
                        current_dialog_file.clear();
                        save_vector[3]="ProdAppsIntroDone";
                        WriteVecToFile(save_vector,"files/save.txt");
                    }
                    talking = false;
                    continue;
                }
                string namemodifiedline = "";
                for (int i=0 ; i<line.size() ; i++) { //modifies the string with the user's name wherever there's an @ symbol
                    if (line[i]=='@') {
                        namemodifiedline+=save_vector[0];
                    }
                    else {
                        namemodifiedline+=line[i];
                    }
                }
                string updatedline = "";
                for (int i=0 ; i<namemodifiedline.size() ; i+=32) {
                    updatedline += namemodifiedline.substr(i,32) + "\n";
                }
                bubble.changeStr(updatedline);
                //cout << "Speaking..." << endl;

                hr = pVoice->Speak(stringToWString(namemodifiedline).c_str(), 0, NULL);
                while (true) { //checks the current speech status and breaks when its finished
                    if (pVoice->GetStatus(NULL, NULL) == S_OK) {
                        break; // Exit the loop once the speech finishes
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Wait a bit before checking again
                }
            }
            //pVoice->Release();
            //::CoUninitialize();
        }
    }
}



int main()
{
    //Debug window loaded in first
    std::thread debug_thread (DebugMenu,std::ref(debug),std::ref(talking));
    debug_thread.detach();


    sf::Font font; //default font that will be used throughout the application
    if (!font.loadFromFile("files/font.ttf")) {
        cout << "Font not found!" << endl;
        return 1;
    }



    string current_dialog_file;
    ifstream dialogue;



    int screenwidth = GetSystemMetrics(SM_CXSCREEN);
    int screenheight = GetSystemMetrics(SM_CYSCREEN);

    bool grabbed = false;
    bool contextmenu = false;
    bool intro_done = false;
    bool sticky_notes_opened = false;
    bool prod_apps_opened = false;

    string task;
    string name;


    sf::RenderWindow dapperware_window(sf::VideoMode(480,630),"DAPPERWARE",sf::Style::None);
    dapperware_window.setPosition(sf::Vector2i(screenwidth-490,screenheight-670));
    dapperware dapperware_assistant;

    sf::CircleShape circle(50);
    circle.setPosition(266,405);
    circle.setFillColor(sf::Color::Transparent);

    speech_bubble bubble("",font);

    std::thread tts_thread(textToSpeech,std::ref(bubble),std::ref(dialogue),std::ref(current_dialog_file));
    tts_thread.detach();

    vector<context_menu_button> context_menu_buttons = {
    context_menu_button("Register Prod. Apps",font,213,215),
    context_menu_button("To-Do List",font,237,239),
    context_menu_button("Task Timer",font,261,263),
        context_menu_button("Set Reminder",font,285,287),
        context_menu_button("Sticky Notes",font,309,311),

        context_menu_button("Text to Speech",font,333,335),
        context_menu_button("Tell me a joke!",font,357,359),
        context_menu_button("Current Level/XP",font,381,383),
        context_menu_button("Shop",font,405,407),

        context_menu_button("Do Not Disturb",font,429,431),
        context_menu_button("Settings",font,453,455),
        context_menu_button("Exit",font,477,479),
    };


    makeWindowTransparent(dapperware_window);

    while (dapperware_window.isOpen()) {
        sf::Vector2i mouseposition = sf::Mouse::getPosition();
        sf::Event event;
        while (dapperware_window.pollEvent(event)) {
            auto click = event.mouseButton;
            // Close event
            if (event.type == sf::Event::EventType::Closed) {
                dapperware_window.close();
                break;
            }

            if (click.button==sf::Mouse::Left) {
                if (event.type == sf::Event::EventType::MouseButtonPressed) {
                    if (circle.getGlobalBounds().contains(click.x,click.y)) {
                        grabbed = true;
                    }
                }
                for (context_menu_button& button : context_menu_buttons) {
                    if (button.returnShape().getGlobalBounds().contains(click.x,click.y) && intro_done) {
                        task = button.click();
                    }
                }
            }
            if (event.type == sf::Event::EventType::MouseButtonReleased) {
                grabbed = false;
            }
            if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                if (event.type == sf::Event::EventType::MouseButtonPressed) {
                    if (circle.getGlobalBounds().contains(click.x,click.y)) {
                        contextmenu = !contextmenu;
                    }
                }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                if (event.type == sf::Event::EventType::KeyPressed) {
                    debug = !debug;
                }

            }
        }
        if (task == "Exit") {
            dapperware_window.close();
            break;
        }
        else if (task == "Sticky Notes" && !sticky_notes_opened && !talking) {
            if (save_vector[2]=="StickyNotesIntroNotDone") { //plays the sticky notes intro for first time users
                dialogue.close();
                dialogue.open("files/dialogue/intros/stickynotesintro.txt");
                current_dialog_file = "sticky notes intro";
                talking = true;
                //ttsFinished = false;
                //dialogueStarted = false;
            }
            sticky_notes_opened=true;
            contextmenu=false;
            std::thread stickynotemanager_thread (StickyNoteManager,screenwidth,screenheight,std::ref(sticky_notes_opened),std::ref(font));
            stickynotemanager_thread.detach();

        }
        else if (task == "Register Prod. Apps" && !prod_apps_opened && !talking) {
            if (save_vector[3]=="ProdAppsIntroNotDone") {
                dialogue.close();
                dialogue.open("files/dialogue/intros/regappsintro.txt");
                current_dialog_file = "register apps intro";
                talking = true;
                //ttsFinished = false;
                //dialogueStarted = false;
            }
            prod_apps_opened = true;
            contextmenu = false;
            std::thread prodapps_thread (ProductivityAppsWindow);
            prodapps_thread.detach();
        }
        if (grabbed && dapperware_window.hasFocus()) {
            dapperware_window.setPosition(sf::Vector2i(mouseposition.x-315,mouseposition.y-450));
        }
        if (save_vector[1]=="IntroNotDone" && !talking) { //triggers the intro for first time users
            dialogue.close();
            dialogue.open("files/dialogue/intros/intro.txt");
            current_dialog_file = "intro";
            talking=true;
            //ttsFinished=false;
            //dialogueStarted=false;
        }
        else if (save_vector[1]=="IntroDone") {
            intro_done = true;
        }

        dapperware_window.clear(sf::Color(0,0,0,0));
        dapperware_assistant.UpdateSprite();
        dapperware_window.draw(dapperware_assistant.getSprite());
        dapperware_window.draw(circle);
        if (contextmenu) { //will draw the context menu if opened
            DrawContextMenuButtons(context_menu_buttons,dapperware_window);
        }
        if (talking && !dramatic_pause) { //shows speech bubble and enables tts voice when tts is enabled
            bubble.draw(dapperware_window);
        }

        dapperware_window.display();
        if (dapperware_window.hasFocus()) {
            makeWindowOnTop(dapperware_window);
        }

        std::thread active_window_thread(FindTheActiveWindow);
        active_window_thread.detach();
    }
}
