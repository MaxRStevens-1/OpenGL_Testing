#ifndef ANIMATOR_CPP
#define ANIMATOR_CPP


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>
#include <map>

#include "AssimpGLMHelpers.h"
#include "model_animation.h"
#include "Bone.hpp"
#include "Animation.hpp"

class Animator {	
public:
    Animator(Animation* Animation) {
        m_CurrentTime = 0.0;
        m_CurrentAnimation = Animation;

        m_FinalBoneMatrices.reserve(100);

        for (int i = 0; i < 100; i++)
            m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
    }
	
    void UpdateAnimation(float dt) {
        m_DeltaTime = dt;
        if (m_CurrentAnimation) {
            // std::cout << "processing" << std::endl;
            m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
            m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
            CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
        }
    }
	
    void PlayAnimation(Animation* pAnimation)
    {
        m_CurrentAnimation = pAnimation;
        m_CurrentTime = 0.0f;
    }
	
    void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
    {
        std::string nodeName = node->name;
        glm::mat4 nodeTransform = node->transformation;
	
        Bone* Bone = m_CurrentAnimation->FindBone(nodeName);
	
        if (Bone)
        {
            Bone->Update(m_CurrentTime);
            nodeTransform = Bone->GetLocalTransform();
        }
	
        glm::mat4 globalTransformation = parentTransform * nodeTransform;
	
        auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end())
        {
            int index = boneInfoMap[nodeName].id;
            glm::mat4 offset = boneInfoMap[nodeName].offset;
            m_FinalBoneMatrices[index] = globalTransformation * offset;
        }
	
        for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(&node->children[i], globalTransformation);
    }
	
    std::vector<glm::mat4> GetFinalBoneMatrices() 
    { 
        return m_FinalBoneMatrices;  
    }
		
private:
    std::vector<glm::mat4> m_FinalBoneMatrices;
    Animation* m_CurrentAnimation;
    float m_CurrentTime;
    float m_DeltaTime;	
};

#endif