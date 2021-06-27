// ********** IMPORTS/INCLUDE ***********
// libraries
#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// other imports
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>      // Image loading Utility function
#include <learnOpengl/camera.h> // Camera class
// not coded yet: #include <learnOpengl/shaders.h>

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

using namespace std; // Standard namespace


// ********** NAMESPACE ***********
// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "Katie's OpenGL Graphics Project (C++)"; // Macro for window title

    // Main GLFW window
    GLFWwindow* gWindow = nullptr;

    // Variables for window width and height
    const int WINDOW_WIDTH = 1200;
    const int WINDOW_HEIGHT = 800;

    // timing
    float gDeltaTime = 0.0f; // time between current frame and last frame
    float gLastFrame = 0.0f;

    // camera 
    // defines where camera is positioned
    Camera gCamera(glm::vec3(0.2f, 4.6f, 19.9f)); // do not change. precalcated to fit scene at 0.2f, 5.6f, 9.9f
    float gLastX = WINDOW_WIDTH / 2.0f;
    float gLastY = WINDOW_HEIGHT / 2.0f;
    bool gFirstMouse = true;

    // Frag shader program IDs
    GLuint gSideTableProgramId;
    GLuint gSideDrawerProgramId;
    GLuint gHouseFloorProgramId;
    GLuint gRugProgramId;
    GLuint gHouseWallProgramId;
    GLuint gHouseDoorProgramId;
    GLuint gPaintingProgramId;
    GLuint gHouseWreathProgramId;
    GLuint gLampBottomProgramId;
    GLuint gLampTopProgramId;
    GLuint gBalloonsProgramId;
    GLuint gCoffeeTableProgramId;
    GLuint gCouchSeatsProgramId;
    GLuint gCouchArmRestsProgramId;
    GLuint gCouchLegsProgramId;

    // Texture
    // defaults
    GLint gTexWrapMode = GL_REPEAT;
    GLint gTexFilterMode = GL_LINEAR;
    // TODO: check into bindless textures later
    // texture IDs
    GLuint texSideTable; // 0
    GLuint texSideDrawer; // 1, image
    GLuint texHouseFloor; // 2
    GLuint texWallpaper; // 3
    GLuint texHouseDoor; // 4, image
    GLuint texPainting; // 5, image
    GLuint texRug; // 6, image
    GLuint texWreath; // 7
    GLuint texMarbleCream; // 8
    GLuint texCottonCream; // 9
    GLuint texFabricRed; // 10
    GLuint texMetalBlack; // 11

    // TODO: check into bindless textures later

    // UV scale of textures
    // check if can define values within shader, draw or mesh later
    glm::vec2 gUVScale;
    glm::vec2 gUVScaleSideTable;
    glm::vec2 gUVScaleSideDrawer;
    glm::vec2 gUVScaleHouseFloor;
    glm::vec2 gUVScaleRug;
    glm::vec2 gUVScaleHouseWall;
    glm::vec2 gUVScaleHouseDoor;
    glm::vec2 gUVScaleWreath;
    glm::vec2 gUVScalePainting;
    glm::vec2 gUVScaleLampTop;
    glm::vec2 gUVScaleLampBottom;
    glm::vec2 gUVScaleBalloons;
    glm::vec2 gUVScaleCoffeeTable;
    glm::vec2 gUVScaleCouchSeats;
    glm::vec2 gUVScaleCouchLegs;
    glm::vec2 gUVScaleCouchArmRests;


    // Stores the GL data relative to a given mesh
    struct GLMesh
    {
        GLuint vao;         // Handle for the vertex array object
        GLuint vbo;         // Handle for the vertex buffer object
        GLuint nVertices;    // Number of indices of the mesh
        GLuint nIndices;    // Number of indices of the mesh
    };

    // mesh data
    GLMesh gMesh;
    GLMesh gMeshSideTable;
    GLMesh gMeshSideDrawer;
    GLMesh gMeshHouseFloor;
    GLMesh gMeshRug;
    GLMesh gMeshHouseWall;
    GLMesh gMeshHouseDoor;
    GLMesh gMeshHouseWreath;
    GLMesh gMeshPainting;
    GLMesh gMeshLampBottom;
    GLMesh gMeshLampTop;
    GLMesh gMeshBalloons;
    GLMesh gMeshCoffeeTable;
    GLMesh gMeshCouchSeats;
    GLMesh gMeshCouchLegs;
    GLMesh gMeshCouchArmRests;

}

// *********** USER-DEFINED FUNCTIONS **********
// initialize libraries and events
bool initializeOGL(int, char* [], GLFWwindow** window);
void rendering();

// create and destroy shaders
bool createShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void destroyShaderProgram(GLuint programId);

// window resize and projection
void resizeWindow(GLFWwindow* window, int width, int height);
void toOrtho();
void toPerspective();

/* Keyboard input for changing projection setting to ortho
 * !!IMPORTANT!! Do not delete unless can switch project elsewhere)
 * Press "p" to change projection from perspective to ortho**
 */
void changeProjectionCallback(); // use keyboard to change projection (event)
void projectionKeyboardCallback(GLFWwindow* window, int key, int action);
void projectionKeyboardCallback2(GLFWwindow* window, int key, int scancode, int action, int mods);

// user Inputs
void mousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void keyboardNavigationCallback(GLFWwindow* window);

// textures
void flipImageVertical(unsigned char* image, int width, int height, int channels);
bool createTexture(const char* filename, GLuint& textureId, GLint gTexWrapMode, GLint gTexFilterMode);
void destroyTexture(GLuint textureId);
// TODO: research batch texture deletion
//void deleteAllTexture(GLuint& textureIdAll, int texCount);


// create mesh
// mesh for cube
// TODO: check for method to destroy as a batch
void destroyMesh(GLMesh& gMesh);
void createMeshSideTable(GLMesh& gMesh);
void createMeshSideDrawer(GLMesh& gMesh);
void createMeshHouseFloor(GLMesh& gMesh);
void createMeshRug(GLMesh& gMesh);
void createMeshHouseWall(GLMesh& gMesh);
void createMeshHouseDoor(GLMesh& gMesh);
void createMeshWreath(GLMesh& gMesh);
void createMeshPainting(GLMesh& gMesh);
void createMeshLapBottom(GLMesh& gMesh);
void createMeshLapTop(GLMesh& gMesh);
void createMeshCoffeeTable(GLMesh& gMesh);
void createMeshBalloons(GLMesh& gMesh);
void createMeshCoffeeTable(GLMesh& gMesh);
void createMeshCouch(GLMesh& gMesh);
void createMeshCouchLegs(GLMesh& gMesh);
void createMeshCouchArmRests(GLMesh& gMesh);


// draw
void drawSideTable(glm::mat4 view, glm::mat4 projection, GLuint shaderProgramID, GLMesh& gMesh, GLenum textureNum, GLuint textureName);
void drawSideDrawer(glm::mat4 view, glm::mat4 projection, GLuint shaderProgramID, GLMesh& gMesh, GLenum textureNum, GLuint textureName);
void drawHouseFloor(glm::mat4 view, glm::mat4 projection, GLuint shaderProgramID, GLMesh& gMesh, GLenum textureNum, GLuint textureName);
void drawRug(glm::mat4 view, glm::mat4 projection, GLuint shaderProgramID, GLMesh& gMesh, GLenum textureNum, GLuint textureName);
void drawHouseWall(glm::mat4 view, glm::mat4 projection, GLuint shaderProgramID, GLMesh& gMesh, GLenum textureNum, GLuint textureName);
void drawHouseDoor(glm::mat4 view, glm::mat4 projection, GLuint shaderProgramID, GLMesh& gMesh, GLenum textureNum, GLuint textureName);
void drawWreath(glm::mat4 view, glm::mat4 projection, GLuint shaderProgramID, GLMesh& gMesh, GLenum textureNum, GLuint textureName);
void drawPainting(glm::mat4 view, glm::mat4 projection, GLuint shaderProgramID, GLMesh& gMesh, GLenum textureNum, GLuint textureName);
void drawLampBottom(glm::mat4 view, glm::mat4 projection, GLuint shaderProgramID, GLMesh& gMesh, GLenum textureNum, GLuint textureName);
void drawLampTop(glm::mat4 view, glm::mat4 projection, GLuint shaderProgramID, GLMesh& gMesh, GLenum textureNum, GLuint textureName); 
void drawCoffeeTable(glm::mat4 view, glm::mat4 projection, GLuint shaderProgramID, GLMesh& gMesh, GLenum textureNum, GLuint textureName);
void drawBalloons(glm::mat4 view, glm::mat4 projection, GLuint shaderProgramID, GLMesh& gMesh, GLenum textureNum, GLuint textureName);
void drawCouch(glm::mat4 view, glm::mat4 projection, GLuint shaderProgramID, GLMesh& gMesh, GLenum textureNum, GLuint textureName);
void drawCouchLegs(glm::mat4 view, glm::mat4 projection, GLuint shaderProgramID, GLMesh& gMesh, GLenum textureNum, GLuint textureName);
void drawCouchArmRests(glm::mat4 view, glm::mat4 projection, GLuint shaderProgramID, GLMesh& gMesh, GLenum textureNum, GLuint textureName);


// ****** VERTEX SHADER SOURCE CODE
// Vertex Shader Source Code
const GLchar* vertexShaderSource = GLSL(440,

    layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
    layout(location = 1) in vec3 normal; // VAP position 1 for normals
    layout(location = 2) in vec2 textureCoordinate;

    out vec3 vertexNormal; // For outgoing normals to fragment shader
    out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
    out vec2 vertexTextureCoordinate;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;


    void main()
    {
        gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates
        vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)
        vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
        vertexTextureCoordinate = textureCoordinate;
    }
);


// ****** FRAGMENT SHADER SOURCE CODE
// UNABLE TO GET VERTEX TO ACCEPT A 2ND/DIFFERENT SET OF COORDINATES
// SOLUTION: create another frag shader (research other opportunities later)

// fragment shader: side table
const GLchar* sideTableFragShader = GLSL(440,
    in vec2 vertexTextureCoordinate;

    out vec4 fragmentColor;

    uniform sampler2D texSideTable;
    uniform vec2 uvScaleSideDresser;
    uniform vec2 uvScaleDresserLegs;

void main()
{
    // https://open.gl/textures
    vec4 fragTexFrame = texture(texSideTable, vertexTextureCoordinate * uvScaleSideDresser);
    //vec4 fragTexLegs = texture(texSideTable, vertexTextureCoordinate * uvScaleDresserLegs);

    if (fragTexFrame.a < 0.1)
        discard;
    fragmentColor = fragTexFrame;
    // fragmentColor = fragtexWoodScratched * fragtexWoodScratchedLegs;
    // fragmentColor = fragtexWoodScratched + fragtexWoodScratchedLegs;
}
);


// fragment shader: side table drawer
const GLchar* sideDrawerFragShader = GLSL(440,
    in vec2 vertexTextureCoordinate;

    out vec4 fragmentColor;
    uniform sampler2D texSideDrawer;
    uniform vec2 uvScaleSideDrawer;

void main()
{
    vec4 fragTex = texture(texSideDrawer, vertexTextureCoordinate * uvScaleSideDrawer);
    if (fragTex.a < 0.1)
        discard;
    fragmentColor = fragTex;
}
);


// fragment shader: house floor 
const GLchar* houseFloorFragShader = GLSL(440,
    in vec2 vertexTextureCoordinate;

out vec4 fragmentColor;

uniform sampler2D texHouseFloor;
uniform vec2 uvScaleHouseFloor;

void main()
{
    vec4 fragTex = texture(texHouseFloor, vertexTextureCoordinate * uvScaleHouseFloor);
    if (fragTex.a < 0.1)
        discard;
    fragmentColor = fragTex;
}
);


// fragment shader: house wall 
const GLchar* houseWallFragShader = GLSL(440,
    in vec2 vertexTextureCoordinate;

out vec4 fragmentColor;

uniform sampler2D texHouseWall;
uniform vec2 uvScaleHouseWall;

void main()
{
    vec4 fragTex = texture(texHouseWall, vertexTextureCoordinate * uvScaleHouseWall);
    if (fragTex.a < 0.1)
        discard;
    fragmentColor = fragTex;
}
);


// fragment shader: house door
const GLchar* houseDoorFragShader = GLSL(440,
    in vec2 vertexTextureCoordinate;

out vec4 fragmentColor;

uniform sampler2D texHouseDoor;
uniform vec2 gUVScaleHouseDoor;

void main()
{
    vec4 fragTex = texture(texHouseDoor, vertexTextureCoordinate * gUVScaleHouseDoor);
    if (fragTex.a < 0.1)
        discard;
    fragmentColor = fragTex;
}
);


/*
// fragment shader: rug
const GLchar* rugFragShader = GLSL(440,
    in vec2 vertexTextureCoordinate;

    out vec4 fragmentColor;

    uniform sampler2D texRug;
    uniform vec2 gUVScaleRug;

    void main()
    {
    vec4 fragTex = texture(texRug, vertexTextureCoordinate * gUVScaleRug);
    if (fragTex.a < 0.1)
        discard;
    fragmentColor = fragTex;
    }
);
*/

// fragment shader: painting
const GLchar* paintingFragShader = GLSL(440,
    in vec2 vertexTextureCoordinate;

out vec4 fragmentColor;

uniform sampler2D texPainting;
uniform vec2 gUVScalePainting;

void main()
{
    vec4 fragTex = texture(texPainting, vertexTextureCoordinate * gUVScalePainting);
    if (fragTex.a < 0.1)
        discard;
    fragmentColor = fragTex;
}
);

/*
// fragment shader: wreath
const GLchar* houseWreathFragShader = GLSL(440,
    in vec2 vertexTextureCoordinate;

    out vec4 fragmentColor;

    uniform sampler2D texWreath;
    uniform vec2 gUVScaleWreath;

    void main()
    {
    vec4 fragTex = texture(texWreath, vertexTextureCoordinate * gUVScaleWreath);
    if (fragTex.a < 0.1)
        discard;
    fragmentColor = fragTex;
    }
);


// fragment shader: lamp bottom
const GLchar* lampBottomFragShader = GLSL(440,
    in vec2 vertexTextureCoordinate;

    out vec4 fragmentColor;

    uniform sampler2D texLampBottom;
    uniform vec2 gUVScaleLampBottom;

    void main()
    {
    vec4 fragTex = texture(texLampBottom, vertexTextureCoordinate * gUVScaleLampBottom);
    if (fragTex.a < 0.1)
        discard;
    fragmentColor = fragTex;
    }
);


// fragment shader: lamp top
const GLchar* lampTopFragShader = GLSL(440,
    in vec2 vertexTextureCoordinate;

    out vec4 fragmentColor;

    uniform sampler2D texLampTop;
    uniform vec2 gUVScaleLampTop;

    void main()
    {
    vec4 fragTex = texture(texLampTop, vertexTextureCoordinate * gUVScaleLampTop);
    if (fragTex.a < 0.1)
        discard;
    fragmentColor = fragTex;
    }
);


// fragment shader: balloons
const GLchar* balloonsFragShader = GLSL(440,
    in vec2 vertexTextureCoordinate;

    out vec4 fragmentColor;

    uniform sampler2D texBalloons;
    uniform vec2 gUVScaleBalloons;

    void main()
    {
    vec4 fragTex = texture(texBalloons, vertexTextureCoordinate * gUVScaleBalloons);
    if (fragTex.a < 0.1)
        discard;
    fragmentColor = fragTex;
    }
);


// fragment shader: coffee table
const GLchar* coffeeTableFragShader = GLSL(440,
    in vec2 vertexTextureCoordinate;

    out vec4 fragmentColor;

    uniform sampler2D texCoffeeTable;
    uniform vec2 gUVScaleCoffeeTable;

    void main()
    {
    vec4 fragTex = texture(texCoffeeTable, vertexTextureCoordinate * gUVScaleCoffeeTable);
    if (fragTex.a < 0.1)
        discard;
    fragmentColor = fragTex;
    }
);


// fragment shader: couch seats
const GLchar* couchSeatsFragShader = GLSL(440,
    in vec2 vertexTextureCoordinate;

    out vec4 fragmentColor;

    uniform sampler2D texCouchSeats;
    uniform vec2 gUVScaleCouchSeats;

    void main()
    {
    vec4 fragTex = texture(texCouchSeats, vertexTextureCoordinate * gUVScaleCouchSeats);
    if (fragTex.a < 0.1)
        discard;
    fragmentColor = fragTex;
    }
);


// fragment shader: couch legs
const GLchar* couchLegsFragShader = GLSL(440,
    in vec2 vertexTextureCoordinate;

    out vec4 fragmentColor;

    uniform sampler2D texCouchLegs;
    uniform vec2 gUVScaleCouchLegs;

    void main()
    {
    vec4 fragTex = texture(texCouchLegs, vertexTextureCoordinate * gUVScaleCouchLegs);
    if (fragTex.a < 0.1)
        discard;
    fragmentColor = fragTex;
    }
);


// fragment shader: couch arm rests
const GLchar* couchArmRestsFragShader = GLSL(440,
    in vec2 vertexTextureCoordinate;

    out vec4 fragmentColor;

    uniform sampler2D texCouchArmRests;
    uniform vec2 gUVScaleCouchArmRests;

    void main()
    {
    vec4 fragTex = texture(texCouchArmRests, vertexTextureCoordinate * gUVScaleCouchArmRests);
    if (fragTex.a < 0.1)
        discard;
    fragmentColor = fragTex;
    }
);
*/

// ***** FLIP IMAGES *****
// flip images vertically
// Images are loaded with Y axis going down, but OpenGL's Y axis goes up
void flipImageVertical(unsigned char* image, int width, int height, int channels)
{
    for (int j = 0; j < height / 2; ++j)
    {
        int index1 = j * width * channels;
        int index2 = (height - 1 - j) * width * channels;

        for (int i = width * channels; i > 0; --i)
        {
            unsigned char tmp = image[index1];
            image[index1] = image[index2];
            image[index2] = tmp;
            ++index1;
            ++index2;
        }
    }
}

// ********** MAIN CLASS ***********
int main(int argc, char* argv[])
{
    // initialize OpenGL, GLFW, glfw events, and GLEW
    if (!initializeOGL(argc, argv, &gWindow))
        return EXIT_FAILURE;

    // Sets the background color of the window to black (it will be implicitely used by glClear)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // create the mesh for objects (Calls the function to create the Vertex Buffer Object)
    // TODO: raise exceptions
    createMeshSideTable(gMeshSideTable);
    createMeshSideDrawer(gMeshSideDrawer);
    createMeshHouseFloor(gMeshHouseFloor);
    //createMeshRug)(gMeshRug);
    createMeshHouseWall(gMeshHouseWall);
    createMeshHouseWall(gMeshHouseDoor);
    createMeshPainting(gMeshPainting);
    /*
    //createMeshWreath(gMeshHouseWreath);
    createMeshLapBottom(gMeshLampBottom);
    createMeshLapTop(gMeshLampTop);
    createMeshBalloons(gMeshBalloons);
    createMeshCoffeeTable(gMeshCoffeeTable);
    createMeshCouchSeats(gMeshCouchSeats);
    createMeshCouchLegs(gMeshCouchLegs);
    createMeshCouchArmRests(gMeshCouchArmRests);
    */

    // create the fragment shader programs
    if (!createShaderProgram(vertexShaderSource, sideTableFragShader, gSideTableProgramId))
        return EXIT_FAILURE;

    if (!createShaderProgram(vertexShaderSource, sideDrawerFragShader, gSideDrawerProgramId))
        return EXIT_FAILURE;

    if (!createShaderProgram(vertexShaderSource, houseFloorFragShader, gHouseFloorProgramId))
        return EXIT_FAILURE;

    /*
    if (!createShaderProgram(vertexShaderSource, rugFragShader, gRugProgramId))
        return EXIT_FAILURE;
    */

    if (!createShaderProgram(vertexShaderSource, houseWallFragShader, gHouseWallProgramId))
        return EXIT_FAILURE;

    if (!createShaderProgram(vertexShaderSource, houseDoorFragShader, gHouseDoorProgramId))
        return EXIT_FAILURE;

    if (!createShaderProgram(vertexShaderSource, paintingFragShader, gPaintingProgramId))
        return EXIT_FAILURE;

    /*
    if (!createShaderProgram(vertexShaderSource, houseWreathFragShader, gHouseWreathProgramId))
        return EXIT_FAILURE;

    if (!createShaderProgram(vertexShaderSource, lampBottomFragShader, gLampBottomProgramId))
        return EXIT_FAILURE;

    if (!createShaderProgram(vertexShaderSource, lampTopFragShader, gLampTopProgramId))
        return EXIT_FAILURE;

    if (!createShaderProgram(vertexShaderSource, balloonsFragShader, gBalloonsProgramId))
        return EXIT_FAILURE;

    if (!createShaderProgram(vertexShaderSource, coffeeTableFragShader, gCoffeeTableProgramId))
        return EXIT_FAILURE;

    if (!createShaderProgram(vertexShaderSource, couchSeatsFragShader, gCouchSeatsProgramId))
        return EXIT_FAILURE;

    if (!createShaderProgram(vertexShaderSource, couchLegsFragShader, gCouchArmRestsProgramId))
        return EXIT_FAILURE;

    if (!createShaderProgram(vertexShaderSource, couchArmRestsFragShader, gCouchLegsProgramId))
        return EXIT_FAILURE;

    */
     

    // TEXTURES
    // TODO: CHECK ABOUT MINIMIZE INTO FUNCTION(S) LATER ??
    
    // texture 0: wood, rustic scratched
    const char* texFilename = "../../resources/textures/wood-scratched.jpg";
    if (!createTexture(texFilename, texSideTable, GL_REPEAT, GL_LINEAR))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    // side tables
    glUseProgram(gSideTableProgramId);
    glUniform1i(glGetUniformLocation(gSideTableProgramId, "texSideTable"), 0);


    // texture 1: (IMAGE) dresser drawer, side tables
    texFilename = "../../resources/images/dresserdrawer.png";
    if (!createTexture(texFilename, texSideDrawer, GL_CLAMP_TO_EDGE, GL_LINEAR))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    // side tables drawers
    glUseProgram(gSideDrawerProgramId);
    glUniform1i(glGetUniformLocation(gSideDrawerProgramId, "texSideDrawer"), 1);


    // texture 2: wood (herring pattern)/dark
    texFilename = "../../resources/textures/wood-floor-herringdark.jpg";
    if (!createTexture(texFilename, texHouseFloor, GL_REPEAT, GL_LINEAR))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    // floor
    glUseProgram(gHouseFloorProgramId);
    glUniform1i(glGetUniformLocation(gHouseFloorProgramId, "texHouseFloor"), 2);


    // texture 3: wallpaper blue
    texFilename = "../../resources/textures/wallpaper-pattern.png";
    if (!createTexture(texFilename, texWallpaper, GL_REPEAT, GL_LINEAR))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    // wall
    glUseProgram(gHouseWallProgramId);
    glUniform1i(glGetUniformLocation(gHouseWallProgramId, "texHouseWall"), 3);


    // texture 4: door
    texFilename = "../../resources/";
    if (!createTexture(texFilename, texHouseDoor, GL_REPEAT, GL_LINEAR))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    //
    glUseProgram(gHouseWallProgramId);
    glUniform1i(glGetUniformLocation(gHouseWallProgramId, "texHouseDoor"), 4);


    // texture 5: painting
    texFilename = "../../resources/";
    if (!createTexture(texFilename, texPainting, GL_REPEAT, GL_LINEAR))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    //
    glUseProgram(gPaintingProgramId);
    glUniform1i(glGetUniformLocation(gPaintingProgramId, "texPainting"), 5);

    /*
    // texture : 
    texFilename = "../../resources/";
    if (!createTexture(texFilename, , GL_REPEAT, GL_LINEAR))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    // 
    glUseProgram();
    glUniform1i(glGetUniformLocation(, ""), );
    */

    // RENDERING LOOP
    // (exit key is esc, defined at "exit" (at end of main class)
    while (!glfwWindowShouldClose(gWindow))
    {
        // per-frame timing
        float currentFrame = glfwGetTime();
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;

        // input
        keyboardNavigationCallback(gWindow);
        // **Press "p" to change projection from perspective to ortho**
        // projectionKeyboardCallback(gWindow, p, );

        // creates and draws all objects
        rendering();

        // poll for user or OS activity
        glfwPollEvents();
    }


    // release mesh data
    // TODO: look into batch release destroy/release
    destroyMesh(gMesh);
    destroyMesh(gMeshSideTable);
    destroyMesh(gMeshSideDrawer);
    destroyMesh(gMeshHouseFloor);
    //destroyMesh(gMeshRug);
    destroyMesh(gMeshHouseWall);
    destroyMesh(gMeshHouseDoor);
    destroyMesh(gMeshPainting);
    /*
    //destroyMesh(gMeshHouseWreath);
    destroyMesh(gMeshLampBottom);
    destroyMesh(gMeshLampTop);
    destroyMesh(gMeshBalloons);
    destroyMesh(gMeshCoffeeTable);
    destroyMesh(gMeshCouchSeats);
    destroyMesh(gMeshCouchLegs);
    destroyMesh(gMeshCouchArmRests);
    */

    // release texture
    // TODO: look into batch release destroy/release. 
    // OpenGL 4.4 destroy textures function (don't forget that "s")
    destroyTexture(texSideTable);
    destroyTexture(texSideDrawer); 
    destroyTexture(texHouseFloor); 
    destroyTexture(texWallpaper); 
    destroyTexture(texHouseDoor); 
    destroyTexture(texPainting);
    /*
    destroyTexture(texRug); // 7 image
    destroyTexture(texWreath); // 8
    destroyTexture(texMarbleCream); // 9
    destroyTexture(texCottonCream); // 10
    destroyTexture(texFabricRed); // 11
    destroyTexture(texMetalBlack); // 12
    destroyTexture(texWoodOakFine); // 13
    destroyTexture(texWalnutMed); // 14
    */

    // release fragment shader programs
    destroyShaderProgram(gSideTableProgramId);
    destroyShaderProgram(gSideDrawerProgramId);
    destroyShaderProgram(gHouseFloorProgramId);
    //destroyShaderProgram(gRugProgramId);
    destroyShaderProgram(gHouseWallProgramId);
    destroyShaderProgram(gHouseDoorProgramId);
    destroyShaderProgram(gPaintingProgramId);
   /*
   *destroyShaderProgram(gHouseWreathProgramId);
    destroyShaderProgram(gLampBottomProgramId);
    destroyShaderProgram(gLampTopProgramId);
    destroyShaderProgram(gBalloonsProgramId);
    destroyShaderProgram(gCoffeeTableProgramId);
    destroyShaderProgram(gCouchSeatsProgramId);
    destroyShaderProgram(gCouchArmRestsProgramId);
    destroyShaderProgram(gCouchLegsProgramId);
    */

    exit(EXIT_SUCCESS); // Terminates the program successfully
}

// ********** INTIALIZE OPENGL AND EVENTS **********
// Initialize GLFW, GLEW, and create a window
bool initializeOGL(int argc, char* argv[], GLFWwindow** window)
{
    // GLFW: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // GLFW: window creation
    * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, resizeWindow);

    // tell GLFW to capture mouse
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(*window, mousePositionCallback);
    glfwSetScrollCallback(*window, mouseScrollCallback);
    glfwSetMouseButtonCallback(*window, mouseButtonCallback);

    // need to test if can specific sticky keys to letter p (or P) only
    //glfwSetInputMode(*window, GLFW_STICKY_KEYS, GLFW_KEY_P);

    glfwSetInputMode(*window, GLFW_STICKY_KEYS, GLFW_TRUE);
    /* change projection when key is pressed:
     * press "p" to change projection from perspective to ortho*/
    //glfwSetKeyCallback(*window, projectionKeyboardCallback2);
    //glfwSetInputMode(*windows, projectionKeyboardCallback);


    // GLEW: initialize
    // Note: if using GLEW version 1.13 or earlier
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult)
    {
        std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
        return false;
    }

    // Displays GPU OpenGL version
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    return true;
}


// ********** WINDOW RESIZE AND PROJECTION SETTINGS **********
// adjusts window when screen size changed is changed (by OS or user)
void resizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

/*
void toOrtho()
{
    glm::mat4 projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
    GLint projLoc = glGetUniformLocation(shaderProgramID, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
}



void toPerspective()
{
    glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    GLint projLoc = glGetUniformLocation(shaderProgramID, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
}
*/

void changeProjectionCallback()
{
    // ADD: change to ortho
    // ADD: reset to original
}


// ********** KEYBOARD-BASED NAVIGATION **********
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void keyboardNavigationCallback(GLFWwindow* window)
{
    static const float cameraSpeed = 2.5f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // move scene backward, forward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
    // move scene left, right
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gCamera.ProcessKeyboard(LEFT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gCamera.ProcessKeyboard(RIGHT, gDeltaTime);
    //move scene down, up
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        gCamera.ProcessKeyboard(UP, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        gCamera.ProcessKeyboard(DOWN, gDeltaTime);
}


// DEFAULT KEYBOARD CALLBACK FOR TESTING!!! NEED TO SWITCH TO NON-2 FOR THE PROJECTION FUNCTION
// testing callbacks
// int scan code is only needed to display the value sent to the computer (similar to the ASCII code)
void projectionKeyboardCallback2(GLFWwindow* window, int key, int scancode, int action, int mods) {

    if (key == GLFW_KEY_P && action == GLFW_PRESS)
        cout << key << endl;
    
};


//  input: keyboard controls (non-navigation)
void projectionKeyboardCallback(GLFWwindow* window, int key, int action) {


    // currently in IhitializeGL class, check if need to move to here later
    //glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    //glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_P)

    // add projection here. call on ortho
    // use callback or sticky keys (not get key)
    // source: https://www.glfw.org/docs/3.3/input_guide.html#input_key
        // "When sticky keys mode is enabled, the pollable state of a key will remain GLFW_PRESS until the state of that key is polled with glfwGetKey."
    // Creates a ortho projection
    
    // test
   // if (key == GLFW_KEY_P && action == GLFW_PRESS)
       // toOrtho();

    /*
    if (key == GLFW_KEY_P)
        switch (action)
        {
        case GLFW_PRESS:

        case GLFW_RELEASE :
        }
    */

}


// ********** MOUSE-BASED NAVIGATION **********
// glfw: mouse movement changes orientation of camera
void mousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (gFirstMouse)
    {
        gLastX = xpos;
        gLastY = ypos;
        gFirstMouse = false;
    }

    float xoffset = xpos - gLastX;
    float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top

    gLastX = xpos;
    gLastY = ypos;

    gCamera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: mouse scroll wheel increase/decreases movement speed
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    gCamera.ProcessMouseScroll(yoffset);
}


// glfw: mouse buttons (test mode. only prints text)
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
    {
        if (action == GLFW_PRESS)
            cout << "Left mouse button pressed" << endl;
        else
            cout << "Left mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
    {
        if (action == GLFW_PRESS)
            cout << "Middle mouse button pressed" << endl;
        else
            cout << "Middle mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_RIGHT:
    {
        if (action == GLFW_PRESS)
            cout << "Right mouse button pressed" << endl;
        else
            cout << "Right mouse button released" << endl;
    }
    break;

    default:
        cout << "Unhandled mouse button event" << endl;
        break;
    }
}


// ********** SHADERS: CREATE AND DESTROY **********
void destroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}


// Implements the UCreateShaders function
bool createShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
    // Compilation and linkage error reporting
    int success = 0;
    char infoLog[512];

    // Create a Shader program object.
    programId = glCreateProgram();

    // Create the vertex and fragment shader objects
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Retrive the shader source
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    // Compile the vertex shader, and print compilation errors (if any)
    glCompileShader(vertexShaderId); // compile the vertex shader
    // check for shader compile errors
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glCompileShader(fragmentShaderId); // compile the fragment shader
    // check for shader compile errors
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // Attached compiled shaders to the shader program
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    glLinkProgram(programId);   // links the shader program
    // check for linking errors
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glUseProgram(programId);    // Uses the shader program

    return true;
}


// ********** TEXTURES/IMAGES **********
// remove once deleteTexture properly functions/enabled
void destroyTexture(GLuint textureId)
{
    glGenTextures(1, &textureId);
}


// generates settings for individual texture 
// assumes all images are declared and processed as GL_TEXTURE_2D
bool createTexture(const char* filename, GLuint& textureId, GLint gTexWrapMode, GLint gTexFilterMode)
{
    int width, height, channels;

    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);

    if (image)
    {
        flipImageVertical(image, width, height, channels);

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gTexWrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gTexWrapMode);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gTexFilterMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gTexFilterMode);

        if (channels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        else if (channels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        else
        {
            cout << "Not implemented to handle image with " << channels << " channels" << endl;
            return false;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(image);
        //added after watching video
        glActiveTexture(0);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

        return true;
    }

    // Error loading the image
    return false;
}


// ********** RENDERING **********
// Functioned called to render a frame
void rendering()
{

    // Clear the frame and z buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Enable z-depth
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = gCamera.GetViewMatrix();

    // DrawLight();

    drawSideTable(view, projection, gSideTableProgramId, gMeshSideTable, GL_TEXTURE0, texSideTable);
    drawSideDrawer(view, projection, gSideDrawerProgramId, gMeshSideDrawer, GL_TEXTURE1, texSideDrawer);
    drawHouseFloor(view, projection, gHouseFloorProgramId, gMeshHouseFloor, GL_TEXTURE2, texHouseFloor);
    drawHouseWall(view, projection, gHouseWallProgramId, gMeshHouseWall, GL_TEXTURE3, texWallpaper);
    drawHouseDoor(view, projection, gHouseDoorProgramId, gMeshHouseDoor, GL_TEXTURE4, texHouseDoor);
    drawPainting(view, projection, gPaintingProgramId, gMeshPainting, GL_TEXTURE5, texPainting);

    // Deactivate the Vertex Array Object and shader program
    glBindVertexArray(0);
    glUseProgram(0);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}


// ********** OBJECT MESH **********
void destroyMesh(GLMesh& gMesh)
{
    glDeleteVertexArrays(1, &gMesh.vao);
    glDeleteBuffers(1, &gMesh.vbo);
}


// look into separating legs vs drawers OR reducing coding
void createMeshSideTable(GLMesh& gMesh)
{
    GLfloat verts[] = {
        // Vertex Positions    // normals  // textures
        // front
        -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        // back
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        // left side
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        // right side
         0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
         // bottom
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        // top
        -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    gMeshSideTable.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &gMesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(gMesh.vao);

    // buffer for vertex data
    glGenBuffers(1, &gMesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, gMesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}


void createMeshSideDrawer(GLMesh& gMesh)
{
    GLfloat verts[] = {
        // Vertex Positions    // normals  // textures
        // place at front of side table dresser cube
        -0.55f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // left bottom
         0.55f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // right bottom
         0.55f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // right top
         0.55f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // right top
        -0.55f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // left top
        -0.55f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f  // left bottom
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    gMesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &gMesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(gMesh.vao);

    // buffer for vertex data
    glGenBuffers(1, &gMesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, gMesh.vbo); // Activates the buffer
    glBindBuffer(GL_ARRAY_BUFFER, gMesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

// mesh
void createMeshHouseFloor(GLMesh& gMesh)
{
    GLfloat verts[] = {
        // Vertex Positions    // normals  // textures
        // place at front of side table dresser cube
        -1.0f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // left back
         1.0f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // right back
         1.0f, 0.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // right front
         1.0f, 0.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // right front
        -1.0f, 0.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // left front
        -1.0f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f  // left back
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    gMesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &gMesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(gMesh.vao);

    // buffer for vertex data
    glGenBuffers(1, &gMesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, gMesh.vbo); // Activates the buffer
    glBindBuffer(GL_ARRAY_BUFFER, gMesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}


void createMeshHouseWall(GLMesh& gMesh)
{
    GLfloat verts[] = {
        // Vertex Positions    // normals  // textures
        // place at front of side table dresser cube
        -1.0f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // left bottom
         1.0f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // right bottom
         1.0f, 1.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // right top
         1.0f, 1.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // right top
        -1.0f, 1.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // left top
        -1.0f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f  // left bottom
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    gMesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &gMesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(gMesh.vao);

    // buffer for vertex data
    glGenBuffers(1, &gMesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, gMesh.vbo); // Activates the buffer
    glBindBuffer(GL_ARRAY_BUFFER, gMesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}


void createMeshPainting(GLMesh& gMesh)
{
    GLfloat verts[] = {
        // Vertex Positions    // normals  // textures
        // front
        -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        // back
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        // left side
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        // right side
         0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
         // bottom
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        // top
        -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    gMesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &gMesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(gMesh.vao);

    // buffer for vertex data
    glGenBuffers(1, &gMesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, gMesh.vbo); // Activates the buffer
    glBindBuffer(GL_ARRAY_BUFFER, gMesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}


void createMeshHouseDoor(GLMesh& gMesh)
{
    GLfloat verts[] = {
        // Vertex Positions    // normals  // textures
        // front
        -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        // back
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        // left side
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        // right side
         0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
         // bottom
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        // top
        -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    gMesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &gMesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(gMesh.vao);

    // buffer for vertex data
    glGenBuffers(1, &gMesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, gMesh.vbo); // Activates the buffer
    glBindBuffer(GL_ARRAY_BUFFER, gMesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}


// ********** DRAW OBJECTS **********
 // SEPARATE LEGS FROM TABLE into 2 functions LATER ON
  //  IMPORTANT: remember to keep in position with drawers (drawSideDrawer)

void drawSideTable(glm::mat4 view, glm::mat4 projection, GLuint shaderProgramID, GLMesh& gMesh, GLenum textureNum, GLuint textureName)
{

    // ********** dresser main cuboid ************************
   // dresser cuboid - 1 per table, currently 2 tables
   // create model view: scale, rotate, translate
    glm::mat4 scale = glm::scale(glm::vec3(1.3f, 1.2f, 1.3f));
    glm::mat4 rotation = glm::rotate(0.0f, glm::vec3(0.0f, 0.1f, 0.0f));
    glm::mat4 translation = glm::translate(glm::vec3(-4.0f, 0.8f, 1.5f));
    glm::vec2 gUVScale(0.25f, 0.25f);


    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // Set the shader to be used
    glUseProgram(shaderProgramID);

    /* sets up loop for multiple tables. not needed yet
    // each table has a unique position
    glm::vec3 tablePosition[] = {
    glm::vec3(-3.5f, 0.1f, 1.0f), // 1st dresser
    // position currently unknown for 2nd dresser
    glm::vec3(-3.5f, 0.1f, 1.0f), // 2nd dresser
    };

    //counts the number of objects
    int tableCount = sizeof(tablePosition) / sizeof(tablePosition[0]);

    // draws each dresser cuboid
    // note: rotation is assumed to be the same. if differs, needs to add/integrate array for rotations for both cuboid and legs
    // for (unsigned int i = 0; i < tableCount; i++)
    {
    */
    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgramID, "view");
    GLint projLoc = glGetUniformLocation(shaderProgramID, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    GLint UVScaleLoc = glGetUniformLocation(shaderProgramID, "uvScaleSideDresser");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(gMesh.vao);

    // bind textures on corresponding texture units
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, gTextureId);
    glActiveTexture(textureNum); // 15
    glBindTexture(GL_TEXTURE_2D, textureName);

    // Activate the VBOs contained within the mesh's VA
    glBindVertexArray(gMesh.vao);
    // draws primary dresser cube
    // Draws the triangles

    glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices);
    // IMPORTANT: uncomment when loop is integrated. closes for loop
    //}

    // ********** side table legs ************************
    // small legs: 4 per table, currently 2 tables

    // ROTATION NOTE: legs uses same rotation as dresser cuboid.
    //                add rotation loop if rotation varies between tables
    // scale for legs (uniform size for all 4 legs)
    scale = glm::scale(glm::vec3(0.15f, 0.3f, 0.2f));
    glm::vec2 gUVScaleLegs(0.25f, 0.25f);

    // each leg has a unique position
    glm::vec3 legPosition[] = {
        // 1st dresser
        glm::vec3(-3.5f, 0.1f, 1.0f), // right front leg
        glm::vec3(-4.5f, 0.1f, 1.0f), // left front leg
        glm::vec3(-3.5f, 0.1f, 2.0f), // right back leg
        glm::vec3(-4.5f, 0.1f, 2.0f) // left back leg
        /* needed for 2nd dresser. position currently unknown
        // 2nd dresser
        glm::vec3(-3.5f, 0.1f, 1.0f), // right front leg
        glm::vec3(-4.5f, 0.1f, 1.0f), // left front leg
        glm::vec3(-3.5f, 0.1f, 2.0f), // right back leg
        glm::vec3(-4.5f, 0.1f, 2.0f) // left back leg
        */
    };

    // counts the number of objects
    int legCount = sizeof(legPosition) / sizeof(legPosition[0]);

    // draws each leg
    for (unsigned int i = 0; i < legCount; i++)
    {
        // recalculates model matrix with new position
        translation = glm::translate(glm::vec3(legPosition[i]));
        model = translation * rotation * scale;

        // Retrieves and passes transform matrices to the Shader program
        modelLoc = glGetUniformLocation(shaderProgramID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // UV scale unique to legs
        UVScaleLoc = glGetUniformLocation(shaderProgramID, "uvScaleDresserLegs");
        glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScaleLegs));

        // Activate the VBOs contained within the mesh's VAO
        glBindVertexArray(gMesh.vao);

        // TODO: change if need to change binding location to implement different UV scales
        // bind textures on corresponding texture units
        glActiveTexture(textureNum); // 15
        glBindTexture(GL_TEXTURE_2D, textureName);

        // Draws the triangles
        glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices);
    }
}


void drawSideDrawer(glm::mat4 view, glm::mat4 projection, GLuint shaderProgramID, GLMesh& gMesh, GLenum textureNum, GLuint textureName)
{

    // ********** dresser main cuboid ************************
   // dresser drawers - 1 per table, currently 2 tables. possible 2 if scaling is off
   // create model view: scale, rotate, translate

    // TODO: ADJUST SCALE and possible translation. preset to sidetable #1
    //scale = glm::scale(glm::vec3(1.8f, 1.8f, 1.8f));
    //rotation = glm::rotate(0.0f, glm::vec3(3.0f, 0.1f, 0.0f));
    //translation = glm::translate(glm::vec3(4.0f, 0.8f, 1.5f));

    glm::mat4 scale = glm::scale(glm::vec3(1.0f, 1.0f, 1.3f));
    glm::mat4 rotation = glm::rotate(0.0f, glm::vec3(0.0f, 0.1f, 0.0f));
    glm::mat4 translation = glm::translate(glm::vec3(-4.0f, 0.8f, 1.51f));
    glm::vec2 gUVScale(1.0f, 1.00f);

    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // Set the shader to be used
    glUseProgram(shaderProgramID);

    /* sets up loop for multiple tables. each to check coordination with legs (below) and drawer function
     // each drawer has a unique position
    glm::vec3 drawerPosition[] = {
    glm::vec3(-3.5f, 0.1f, 1.0f), // 1st dresser
    // position currently unknown for 2nd dresser
    glm::vec3(-3.5f, 0.1f, 1.0f), // 2nd dresser
    };

    // counts the number of objects
    int drawerCount = sizeof(drawerPosition) / sizeof(drawerPosition[0]);

    // draws dresser draw for each table
    // note: rotation is assumed to be the same. if differs, needs to add/integrate array for rotations for both cuboid and legs
    // for (unsigned int i = 0; i < drawerCount; i++)
    {
    */
    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgramID, "view");
    GLint projLoc = glGetUniformLocation(shaderProgramID, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    GLint UVScaleLoc = glGetUniformLocation(shaderProgramID, "uvScaleSideDrawer");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // bind textures on corresponding texture units
    glActiveTexture(textureNum);
    glBindTexture(GL_TEXTURE_2D, textureName);

    // Activate the VBOs contained within the mesh's VA
    glBindVertexArray(gMesh.vao);
    // draws primary dresser cube
    // 
    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices);
    // IMPORTANT: uncomment when loop is integrated. close for loop
    //}
}


void drawHouseFloor(glm::mat4 view, glm::mat4 projection, GLuint shaderProgramID, GLMesh& gMesh, GLenum textureNum, GLuint textureName)
{
    glm::mat4 scale = glm::scale(glm::vec3(20.0f, 0.2f, 20.0f));
    glm::mat4 rotation = glm::rotate(0.0f, glm::vec3(0.0f, 0.1f, 0.0f));
    glm::mat4 translation = glm::translate(glm::vec3(0.0f, 0.0f, 10.0f));
    glm::vec2 gUVScale(8.0f, 5.00f);

    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // Set the shader to be used
    glUseProgram(shaderProgramID);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgramID, "view");
    GLint projLoc = glGetUniformLocation(shaderProgramID, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    GLint UVScaleLoc = glGetUniformLocation(shaderProgramID, "uvScaleHouseFloor");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // bind textures on corresponding texture units
    glActiveTexture(textureNum); // 15
    //  glBindTexture(GL_TEXTURE_2D, texWoodHerring);
    glBindTexture(GL_TEXTURE_2D, textureName);

    // Activate the VBOs contained within the mesh's VA
    glBindVertexArray(gMesh.vao);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices);
}

void drawHouseWall(glm::mat4 view, glm::mat4 projection, GLuint shaderProgramID, GLMesh& gMesh, GLenum textureNum, GLuint textureName)
{
    glm::mat4 scale = glm::scale(glm::vec3(20.0f, 15.0f, -0.1f));
    glm::mat4 rotation = glm::rotate(0.0f, glm::vec3(0.5f, 0.1f, 0.0f));
    glm::mat4 translation = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
    glm::vec2 gUVScale(30.0f, 20.0f);


    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // Set the shader to be used
    glUseProgram(shaderProgramID);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgramID, "view");
    GLint projLoc = glGetUniformLocation(shaderProgramID, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    GLint UVScaleLoc = glGetUniformLocation(shaderProgramID, "uvScaleHouseWall");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // bind textures on corresponding texture units
    glActiveTexture(textureNum); // 15
    glBindTexture(GL_TEXTURE_2D, textureName);

    // Activate the VBOs contained within the mesh's VA
    glBindVertexArray(gMesh.vao);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices);
}


void drawHouseDoor(glm::mat4 view, glm::mat4 projection, GLuint shaderProgramID, GLMesh& gMesh, GLenum textureNum, GLuint textureName) 
{
    glm::mat4 scale = glm::scale(glm::vec3(5.0f, 5.0f, 2.1f));
    glm::mat4 rotation = glm::rotate(10.0f, glm::vec3(0.5f, 0.1f, 0.0f));
    glm::mat4 translation = glm::translate(glm::vec3(2.0f, 2.0f, -1.0f));
    glm::vec2 gUVScale(30.0f, 20.0f);


    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // Set the shader to be used
    glUseProgram(shaderProgramID);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgramID, "view");
    GLint projLoc = glGetUniformLocation(shaderProgramID, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    GLint UVScaleLoc = glGetUniformLocation(shaderProgramID, "gUVScaleHouseDoor");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // bind textures on corresponding texture units
    glActiveTexture(textureNum); // 15
    glBindTexture(GL_TEXTURE_2D, textureName);

    // Activate the VBOs contained within the mesh's VA
    glBindVertexArray(gMesh.vao);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices);
}


void drawPainting(glm::mat4 view, glm::mat4 projection, GLuint shaderProgramID, GLMesh& gMesh, GLenum textureNum, GLuint textureName) {
    
    glm::mat4 scale = glm::scale(glm::vec3(5.0f, 5.0f, 2.1f));
    glm::mat4 rotation = glm::rotate(2.0f, glm::vec3(0.5f, 0.1f, 0.0f));
    glm::mat4 translation = glm::translate(glm::vec3(1.0f, 1.0f, 1.0f));
    glm::vec2 gUVScale(30.0f, 20.0f);


    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // Set the shader to be used
    glUseProgram(shaderProgramID);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgramID, "view");
    GLint projLoc = glGetUniformLocation(shaderProgramID, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    GLint UVScaleLoc = glGetUniformLocation(shaderProgramID, "gUVScalePainting");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // bind textures on corresponding texture units
    glActiveTexture(textureNum); // 15
    glBindTexture(GL_TEXTURE_2D, textureName);

    // Activate the VBOs contained within the mesh's VA
    glBindVertexArray(gMesh.vao);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices);
    
}


/*
void enableView(GLFWwindow* window);
void switchMVProjection(GLFWwindow* window, int key, int scancode, int action, int mods);

void enableView(GLFWwindow* window)
{    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // Clear the frame and z buffers
    // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Transforms the camera: move the camera
    // stationary glm::mat4 view = glm::translate(glm::vec3(0.0f, 0.0f, -5.0f));
    // camera/view transformation
    glm::mat4 view = gCamera.GetViewMatrix();
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // defaulted to perspective projection
    glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    GLint projLoc = glGetUniformLocation(gProgramId, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


    // switched between Ortho and Perspective projection via key callback
    // source: https://www.glfw.org/docs/latest/input_guide.html#input_keyboard

     // switches between Ortho and Perspective projection via key callback
   // glfwSetKeyCallback(window, switchMVProjection);

    // code does responsed, but only when holding key P. Uncomment to test
if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
{
    glm::mat4 projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
    projLoc = glGetUniformLocation(gProgramId, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

// Set the shader to be used
glUseProgram(gProgramId);
};

*/