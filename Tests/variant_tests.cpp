#include "./test_app.hpp"
#include "./variant_tests.hpp"
#include "../Nostalgia/managers/manager.hpp"
#include "../Nostalgia/theatre/theatre_file.hpp"
#include "../Nostalgia/things/thing_data.hpp"
#include "../Nostalgia/things/thing_factory.hpp"

extern std::string _hello_world_nt;

int TestApplication::Main()
{
    ThingFactory::Init();

    TheatreFile::TokenArray _tokens{};
    auto _data{MakeShared<TheatreFile::TheatreData2>()};
    TheatreFile::Lex(FileData{_hello_world_nt, FileType::theatre_NT}, _tokens);
    TheatreFile::Parse(_tokens, _data);
    std::println("{}", _data->get_log());
    return 0;
}

void TestApplication::Stop()
{ Manager::Stop(); }

const char* TestApplication::Name()
{ return "Variant Testing Application"; }

void TestApplication::Input(InputEvent*)
{}

void TestApplication::Event(IEvent*)
{}

std::string _hello_world_nt{R"~(
    @HelloWorld#0

    declare EditorPlayer3D inherits Actor3D
    declare TestAnimatedSprite2D inherits Sprite2D

    ImageTexture StupidTex
    {
        Image = 'testing_resources/Stupid.png'
    }

    ArrayMesh IcoSphere
    {
        Material = <StupidMaterial>
        Model = 'testing_resources/IcoSphere.obj'
    }
    Material LolBitMaterial
    {
        DiffuseTexture <LolBitTexture>
        SamplerMipFilterMin = (SamplerFilterNearest)
    }

    MeshInstance3D LolBitCubeMeshInstance3D
    {
        MaterialOverride = <LolBitMaterial>
        Mesh = <DefaultCube>
    }

    Material RedMaterial
    {
        NoTexture = [trUe]
        Color = [1, 0.2, 0.1]
        SpecularStrength = [0.6]
    }

    Actor3D LolBitCube
    {
        Child = <LolBitCubeMeshInstance3D>
        Position = [-0.25, 2, -3.5]
        RotationDegrees = [25, 0, 1]
    }

    Collider3D DoomCube
    {
        Child : MeshInstance3D DoomCubeMeshInstance3D
        {
            Mesh = <DefaultCube>
            MaterialOverride = <DoomMaterial>
        }
        Motion = (Dynamic)
        Position = [3.01, -0.1, -3]
        RotationDegrees = [-20, 20, 50]
        Visible = [true]
    }

    NostalgiaPlayer Player
    {
        Position = [0, 0.3, 6]
        Actor3D = <Player_EditorActor3D>
        Child : EditorPlayer3D Player_EditorActor3D {}
    }

    Material StupidMaterial
    {
        Color = [0.9, 1, 1]
        SpecularStrength = [0.2]
        DiffuseTexture = <StupidTex>
    }

    Material DoomMaterial
    { DiffuseTexture = <DoomTexture> }

    Ramiel Ramiel
    {
        Position = [-30, 18.152, -215]
        Direction = [-0.05, 0.01, 1]
        Speed = [0.01]
    }

    Collider3D Floor
    {
        Position = [3, -5, -3]
        Scale = [20, 2, 20]
        Child : MeshInstance3D FloorMesh
        {
            Mesh = <DefaultCube>
            MaterialOverride = <RedMaterial>
        }
    }

    Collider3D DoomCube2
    {
        Child : MeshInstance3D DoomCubeMeshInstance3D2
        {
            Mesh = <DefaultCube>
            MaterialOverride = <DoomMaterial>
        }
        Motion = (Dynamic)
        Position = [2.35, 6.95, -2.95]
        RotationDegrees = [-36.6, 25, 0.1]
        Scale = [0.3, 1, 1.7]
        Visible = [true]
    }

    Collider3D DoomCube3
    {
        Motion = (Dynamic)
        Position = [3, 5, -3]
        RotationDegrees = [20, -20, -50]
        Scale = [0.5, 1, 0.7]
        Visible = [true]

        Child : MeshInstance3D DoomCubeMeshInstance3D3
        {
            Mesh = <DefaultCube>
            MaterialOverride = <LolBitMaterial>
        }
    }

    Actor3D RedCube
    {
        Child : MeshInstance3D RedCubeMeshInstance3D
        {
            Mesh = <DefaultCube>
            MaterialOverride = <RedMaterial>
        }
        Position = [1.35, 1, -4.45]
        RotationDegrees = [-0.3, -64.6, -17.4]
        OuuughImSoBigAndRound = [0.48, 0.58, 0.27]
        Visible = [true]
    }

    Actor3D IcoSphereActor3D
    {
        Child : MeshInstance3D IcoSphereMeshInstance3D
        { Mesh = <IcoSphere> }
        Position = [-2.55, 0.15, -1.3]
        RotationDegrees = [15.2, 38.6, 0]
    }

    PointLight3D PointLight
    {
        Position = [1.6, 0.1, -2.1]
        Color = [1, 0.8, 0.2]
    }

    SpotLight3D SpotLight
    {
        Position = [-0.32, 0.29, -0.45]
        RotationDegrees = [27.6, 1.10, 10.1]
        Color = [0.287, 1.0, 0.368]
        Energy = [0.95]
        Attenuation = [0.9]
        Angle = [12.3]
        AngleFade = [2]
    }

    DirectionalLight3D TheSun
    {
        RotationDegrees = [180, 100, -25]
        Energy = [0.6]
    }

    ImageTexture TestTexture1
    { Image = 'testing_resources/TestTexture1.png' }

    ImageTexture TestTexture2
    { Image = 'testing_resources/TestTexture2.png' }

    Actor2D TestSpriteParent
    {
        Position = [850, -300]
        Child = <TestSprite>
        Child = <TestSpriteCaption>
    }

    TestAnimatedSprite2D TestSprite
    {
        Texture0 = <TestTexture1>
        Texture1 = <TestTexture2>
        Texture2 = <DoomTexture>
        Position = [250, -250]
        Size = [2, 2]
        Interval = [50]
    }

    Text2D TestSpriteCaption
    {
        Position = [118, -400]
        Color = [1, 1, 1]
        Size = [0.5, 0.5]
        Text = 'This ImageTexture is animated.'
    }

    Font TestFont1
    { Font = 'testing_resources/Flynn.otf' }

    Text2D TestText1
    {
        Font = <TestFont1>
        Position = [100, -50]
        Color = [0.2, 1.0, 0.1]
        Text = 'Some text in an imported font.'
    }

    Text2D TestText2
    {
        Position [100, -100]
        Color = [0.2, 1.0, 0.1]
        Text = "Some text in the default font."
    }

    Text2D TestText3
    {
        Position = [100, -150]
        Color = [0.2, 1.0, 0.1]
        Text = 'Some text with debug outlines.'
        Outline = [true]
    }

    Text2D TestText4
    {
        Position = [100, -200]
        Color = [0.2, 1.0, 0.1]
        Text = 'Some text rendered solid.'
        Solid = [true]
    }

    Text2D TestText5
    {
        Position = [100, -250]
        Color = [0.2, 1.0, 0.1]
        Text = 'Some text rendered solid and in wireframe.'
        Solid = [true]
        Wireframe = [true]
    }

    Text2D TheSopranos
    {
        Font = <Verdana>
        Position = [1275, -750]
        Size = [0.9,1]
        Color = [0.2, 1.0, 0.1]
        Text = "He was gay, Gary Cooper?"
    }

    Material Test1Mat
    { DiffuseTexture = <TestTexture1> }

    Material Test2Mat
    { DiffuseTexture = <TestTexture2> }

    Actor3D TestParentActor3D
    {
        Position = [-10, 3, -3]
        Child = <TestChildActor3D>
        Child : MeshInstance3D TestParentMeshInstance3D
        {
            Mesh = <DefaultCube>
            MaterialOverride = <Test1Mat>
        }
    }

    Actor3D TestChildActor3D
    {
        Position = [4, -2, 0]
        Child : MeshInstance3D TestChildMeshInstance3D
        {
            Mesh = <DefaultCube>
            MaterialOverride = <Test2Mat>
        }
    }

    Viewport TestViewport
    {
        ViewportSize = [1920, 1080]

        Child : Camera2D TestViewportCamera2D
        { Position = [-1920, 1080] }

        Child : Text2D TestText2DThing
        {
            Font = <Verdana>
            Position = [-1900, 960]
            Color = [1, 1, 1]
            Scale = [1.8, 2]
            Text = "Hello, Sprite3D! (Hello, ViewportTexture!)"
        }

        Child : Camera3D TestViewportCamera3D
        {
            Position = [4, 2.05, 2]
            RotationDegrees = [-17.5, 44.40, 0]
            UseDefaultSkybox = [true]
        }
    }

    ViewportTexture TestViewportTexture
    { Viewport = <TestViewport> }

    Sprite3D TestViewportTextureTester
    {
        Texture = <TestViewportTexture>
        Position = [4.98, 4.86, -1.8]
        RotationDegrees = [0, -36.12, 0]
        Scale = [1.47, 1.47, 1.47]
    }
)~"};
