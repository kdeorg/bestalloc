/*! ---------------------------------------------------------------
 * \file   BestAllocAlgo.h
 * \brief  BestAllocAlgorithm class declaration
 * \author Edward Ryabikov
 *
 * PROJ: bestalloc
 * ---------------------------------------------------------------- */

#ifndef BESTALLOCALGO_H_INCLUDED
#define BESTALLOCALGO_H_INCLUDED

#include "Bigraph.h"
#include "Employee.h"
#include "Skill.h"

#include <map>
#include <vector>
#include <list>
#include <utility>
using namespace std;

namespace test{
    class Test_BestAllocAlgo;
}

namespace bestalloc
{
    class BestAllocAlgorithm
    {
    private:
        Bigraph<Employee, Skill> m_graph;

        vector< vector<int> > m_effMatrix; // Efficiency matrix
        vector<int>           m_pairUV;    // Matching: pairUV[employee]
        vector<int>           m_pairVU;    // Matching: pairVU[technology]
        vector<bool>          m_altTreeUV; // Alternating tree
        vector<bool>          m_altTreeVU; // Alternating tree
        vector<int>           m_maxRow;
        vector<int>           m_minCol;

    private:
        const Employee* getEmployeeById(int id);
        const Skill*    getSkillById(int id);
        bool isSkillExists(int emplId, int skillId);
        int  getSkillLevel(int emplId, int skillId);

        void initEffMatrix();
        bool buildAltTree(int i);

    public:
        BestAllocAlgorithm();

        void addEmployee(const Employee& employee);
        void addSkill(const Skill& skill);
        void assignSkill(int emplId, int techId, int level);

        vector< pair<Employee, Skill> > getBestAllocation();

        void clearContainers();

        ~BestAllocAlgorithm();

        friend class test::Test_BestAllocAlgo;
    };
}

#endif // BESTALLOCALGO_H_INCLUDED
