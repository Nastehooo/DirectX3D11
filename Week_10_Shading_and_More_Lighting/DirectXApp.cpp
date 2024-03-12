#include "DirectXApp.h"
#include "CubeNode.h"
#include "TextureCubeNode.h"
#include "TeapotNode.h"


DirectXApp app;
shared_ptr<CubeNode> cube;
shared_ptr<TextureCubeNode> textcube;
shared_ptr<TeapotNode> teapot;

void DirectXApp::CreateSceneGraph()
{
    SceneGraphPointer sceneGraph = GetSceneGraph();


    // Create a scene graph for the teapot
    SceneGraphPointer teapotSceneGraph = std::make_shared<SceneGraph>(L"TeapotScene");
    sceneGraph->Add(teapotSceneGraph);

    //Teapot
    shared_ptr<TeapotNode> teapot = make_shared<TeapotNode>(L"Teapot", Vector4(0.5f, 0.0f, 0.5f, 1.0f));
    teapot->SetWorldTransform(Matrix::CreateScale(Vector3(1, 1,  1)) * Matrix::CreateTranslation(Vector3(20, 15, 0)));
    teapotSceneGraph->Add(teapot);

    //Texture for body 
    shared_ptr<TextureCubeNode> textcube = make_shared<TextureCubeNode>(L"TextCubeBody", Vector4(1.0f, 0.0f, 0.0f, 1.0f)); 
    textcube->SetWorldTransform(Matrix::CreateScale(Vector3(5.0f, 8.0f, 2.5f)) * Matrix::CreateTranslation(Vector3(0, 23.0f, 0)));
    sceneGraph->Add(textcube);

    // Body
    shared_ptr<CubeNode> cube = make_shared<CubeNode>(L"Body", Vector4(1.0f, 0.0f, 0.0f, 1.0f)); 
    cube->SetWorldTransform(Matrix::CreateScale(Vector3(1.0f, 7.5f, 1.0f)) * Matrix::CreateTranslation(Vector3(0.0f, 23.0f, 0.0f)));
    sceneGraph->Add(textcube);

    //Texture for left Leg
    shared_ptr<TextureCubeNode> textcube1 = make_shared<TextureCubeNode>(L"TextCubeLeftLeg", Vector4(1.0f, 0.0f, 0.0f, 1.0f)); 
    textcube1->SetWorldTransform(Matrix::CreateScale(Vector3(1.0f, 7.5f, 1.0f)) * Matrix::CreateTranslation(Vector3(-4.0f, 7.5f, 0.0f)));
    sceneGraph->Add(textcube1);

    // Left Leg
    cube  = make_shared<CubeNode>(L"LeftLeg", Vector4(0.8f, 0.6f, 0.4f, 1.0f)); 
    cube->SetWorldTransform(Matrix::CreateScale(Vector3(1.0f, 7.5f, 1.0f)) * Matrix::CreateTranslation(Vector3(-4.0f, 7.5f, 0.0f)));
    sceneGraph->Add(textcube1);

    //Texture for Right Leg
    shared_ptr<TextureCubeNode> textcube2 = make_shared<TextureCubeNode>(L"TextCubeLeftLeg", Vector4(1.0f, 0.0f, 0.0f, 1.0f)); 
    textcube2->SetWorldTransform(Matrix::CreateScale(Vector3(1.0f, 7.5f, 1.0f)) * Matrix::CreateTranslation(Vector3(4.0f, 7.5f, 0.0f)));
    sceneGraph->Add(textcube2);

    // Right Leg
    cube = make_shared<CubeNode>(L"RightLeg", Vector4(0.8f, 0.6f, 0.4f, 1.0f)); 
    cube->SetWorldTransform(Matrix::CreateScale(Vector3(1.0f, 7.5f, 1.0f)) * Matrix::CreateTranslation(Vector3(4.0f, 7.5f, 0.0f)));
    sceneGraph->Add(textcube2);


    // Head
    cube = make_shared<CubeNode>(L"Head", Vector4(0.8f, 0.6f, 0.4f, 1.0f)); 
    cube->SetWorldTransform(Matrix::CreateScale(Vector3(3.0f, 3.0f, 3.0f)) * Matrix::CreateTranslation(Vector3(0.0f, 34.0f, 0.0f)));
    sceneGraph->Add(cube);

    
    // Create a scene graph for the left shoulder
    SceneGraphPointer leftShoulderSceneGraph = std::make_shared<SceneGraph>(L"LeftShoulder");
    leftShoulderSceneGraph->SetWorldTransform(Matrix::CreateTranslation(Vector3(-shoulderOffsetX, shoulderOffsetY, shoulderOffsetZ)));
    sceneGraph->Add(leftShoulderSceneGraph);
    


    // Create a scene graph for the right shoulder
    SceneGraphPointer rightShoulderSceneGraph = std::make_shared<SceneGraph>(L"RightShoulder");
    rightShoulderSceneGraph->SetWorldTransform(Matrix::CreateTranslation(Vector3(shoulderOffsetX, shoulderOffsetY, shoulderOffsetZ)));
    sceneGraph->Add(rightShoulderSceneGraph);
   
    // Left Arm
    cube= make_shared<CubeNode>(L"LeftArm", Vector4(0.8f, 0.6f, 0.4f, 1.0f)); //brown
    cube->SetWorldTransform(Matrix::CreateTranslation(Vector3(-6.0f, 22.0f, 0.0f)));
    leftShoulderSceneGraph->Add(cube);

    // Right Arm
    cube = make_shared<CubeNode>(L"RightArm", Vector4(0.8f, 0.6f, 0.4f, 1.0f));  //brown
    cube->SetWorldTransform(Matrix::CreateTranslation(Vector3(6.0f, 22.0f, 0.0f)));
    rightShoulderSceneGraph->Add(cube);
   

    _rotationAngle = 0;
    _yOffset = 0.0f;


}


void DirectXApp::UpdateSceneGraph()
{

    shoulderOffsetX = 0.0f;
    shoulderOffsetY = -4.25;
    shoulderOffsetZ = 0.0f;

    SceneGraphPointer sceneGraph = GetSceneGraph();

    // Apply rotation to the entire robot
    _rotationAngle += 0.5f;
    Matrix rotationMatrix = Matrix::CreateRotationY(_rotationAngle * XM_PI / 180.0f);
    sceneGraph->SetWorldTransform(rotationMatrix);

    // Define rotation angles for the arms
    float leftArmRotation = sin(_rotationAngle * XM_PI / 180.0f) * 180.0f;  // Swinging left arm
    float rightArmRotation = -sin(_rotationAngle * XM_PI / 180.0f) * 180.0f;  // Swinging right arm

    // Find and update the left arm node
    SceneNodePointer leftShoulderNode = sceneGraph->Find(L"LeftShoulder");
    if (leftShoulderNode) {
        // Set world transformation for the left arm directly attached to the body
        Matrix leftShoulderTransform =
            Matrix::CreateTranslation(Vector3(-shoulderOffsetX, shoulderOffsetY, shoulderOffsetZ))
            * Matrix::CreateRotationX(leftArmRotation * XM_PI / 180.0f)
            * Matrix::CreateTranslation(Vector3(-6.0f, 30.0f, 0.0f));

        leftShoulderNode->SetWorldTransform(leftShoulderTransform);


        // Find and update the left arm node
        SceneNodePointer leftArmNode = sceneGraph->Find(L"LeftArm");
        if (leftArmNode) {
            leftArmNode->SetWorldTransform(Matrix::CreateScale(Vector3(1.0f, 8.5f, 1.0f)) * Matrix::CreateTranslation(Vector3(0, -4.25f, 0)) * Matrix::CreateRotationY(leftArmRotation * XM_PI / 180.0f));

        }

    }


    // Find and update the right arm node
    SceneNodePointer rightShoulderNode = sceneGraph->Find(L"RightShoulder");
    if (rightShoulderNode) {
        // Set world transformation for the right arm directly attached to the body
        Matrix rightShoulderTransform =
            Matrix::CreateTranslation(Vector3(shoulderOffsetX, shoulderOffsetY, shoulderOffsetZ))
            * Matrix::CreateRotationX(rightArmRotation * XM_PI / 180.0f)
            * Matrix::CreateTranslation(Vector3(6.0f, 30.0f, 0.0f));

        rightShoulderNode->SetWorldTransform(rightShoulderTransform);


        // Find and update the right arm node
        SceneNodePointer rightArmNode = sceneGraph->Find(L"RightArm");
        if (rightArmNode) {
            rightArmNode->SetWorldTransform(Matrix::CreateScale(Vector3(1.0f, 8.5f, 1.0f)) * Matrix::CreateTranslation(Vector3(0, -4.25f, 0)) * Matrix::CreateRotationY(leftArmRotation * XM_PI / 180.0f));
        }
    }
    
    // Find and update the teapot node
    SceneNodePointer teapotNode = sceneGraph->Find(L"TeapotScene");
    if (teapotNode) {
        // Update teapot transformation to rotate around its own center
        teapotNode->SetWorldTransform(
            Matrix::CreateRotationY(_rotationAngle * XM_PI / 180.0f) 
        );
    }

 }







       