
/* Portions copyright (c) 2006 Stanford University and Simbios.
 * Contributors: Pande Group
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS, CONTRIBUTORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __AmoebaReferenceVdwForce_H__
#define __AmoebaReferenceVdwForce_H__

#include "SimTKUtilities/SimTKOpenMMRealType.h"
#include "openmm/Vec3.h"
#include <string>

using namespace OpenMM;

class AmoebaReferenceVdwForce;
typedef  RealOpenMM (AmoebaReferenceVdwForce::*CombiningFunction)( RealOpenMM x, RealOpenMM y) const;

// ---------------------------------------------------------------------------------------

class AmoebaReferenceVdwForce {

public:

    /** 
     * This is an enumeration of the different methods that may be used for handling long range Vdw forces.
     */
    enum NonbondedMethod {

        /**
         * No cutoff is applied to the interactions.  The full set of N^2 interactions is computed exactly.
         * This necessarily means that periodic boundary conditions cannot be used.  This is the default.
         */

        NoCutoff = 0,

        /**
         * Interactions beyond the cutoff distance are ignored.  
         */
        CutoffNonPeriodic = 1,
        /**
         * Periodic boundary conditions are used, so that each particle interacts only with the nearest periodic copy of
         * each other particle.  Interactions beyond the cutoff distance are ignored.  
         */
        CutoffPeriodic = 2,
    };
 
    /**---------------------------------------------------------------------------------------
       
       Constructor
       
       --------------------------------------------------------------------------------------- */
 
    AmoebaReferenceVdwForce( );
 
    /**---------------------------------------------------------------------------------------
       
       Constructor
       
       --------------------------------------------------------------------------------------- */
 
    AmoebaReferenceVdwForce( const std::string& sigmaCombiningRule,
                             const std::string& epsilonCombiningRule,
                             NonbondedMethod nonbondedMethod );
 
    /**---------------------------------------------------------------------------------------
       
       Destructor
       
       --------------------------------------------------------------------------------------- */
 
    ~AmoebaReferenceVdwForce( ){};
 
    /**---------------------------------------------------------------------------------------
    
       Get nonbonded method
    
       @return nonbonded method
    
       --------------------------------------------------------------------------------------- */
    
    NonbondedMethod getNonbondedMethod( void ) const;

    /**---------------------------------------------------------------------------------------
    
       Set nonbonded method
    
       @param nonbonded method
    
       --------------------------------------------------------------------------------------- */
    
    void setNonbondedMethod( NonbondedMethod nonbondedMethod );

    /**---------------------------------------------------------------------------------------
    
       Set sigma combining rule
    
       @param sigmaCombiningRule      rule: GEOMETRIC, CUBIC-MEAN, ARITHMETIC (default)
    
       --------------------------------------------------------------------------------------- */
    
    void setSigmaCombiningRule( const std::string& sigmaCombiningRule );

    /**---------------------------------------------------------------------------------------
    
       Get sigma combining rule
    
       @return sigmaCombiningRule
    
       --------------------------------------------------------------------------------------- */
    
    std::string getSigmaCombiningRule( void ) const;

    /**---------------------------------------------------------------------------------------
    
       Set epsilon combining rule
    
       @param epsilonCombiningRule      rule: GEOMETRIC, CUBIC-MEAN, ARITHMETIC (default)
    
       --------------------------------------------------------------------------------------- */
    
    void setEpsilonCombiningRule( const std::string& epsilonCombiningRule );

    /**---------------------------------------------------------------------------------------
    
       Get epsilon combining rule
    
       @return epsilonCombiningRule
    
       --------------------------------------------------------------------------------------- */
    
    std::string getEpsilonCombiningRule( void ) const;

    /**---------------------------------------------------------------------------------------
    
       Calculate Amoeba Hal vdw ixns
    
       @param numParticles            number of particles
       @param particlePositions       Cartesian coordinates of particles
       @param indexIVs                position index for associated reducing particle
       @param indexClasses            class index for combining sigmas/epsilons (not currently used)
       @param sigmas                  particle sigmas 
       @param epsilons                particle epsilons
       @param reductions              particle reduction factors
       @param vdwExclusions           particle exclusions
       @param forces                  add forces to this vector
    
       @return energy
       @return energy
    
       --------------------------------------------------------------------------------------- */
    
    RealOpenMM calculateForceAndEnergy( int numParticles, RealOpenMM** const particlePositions, 
                                        const std::vector<int>& indexIVs, 
                                        const std::vector<int>& indexClasses, 
                                        const std::vector<RealOpenMM>& sigmas, const std::vector<RealOpenMM>& epsilons,
                                        const std::vector<RealOpenMM>& reductions,
                                        const std::vector< std::vector<int> >& vdwExclusions,
                                        RealOpenMM** forces ) const;
         
private:

    std::string _sigmaCombiningRule;
    std::string _epsilonCombiningRule;
    NonbondedMethod _nonbondedMethod;

    CombiningFunction _combineSigmas;
    RealOpenMM arithmeticSigmaCombiningRule( RealOpenMM sigmaI, RealOpenMM sigmaJ ) const;
    RealOpenMM  geometricSigmaCombiningRule( RealOpenMM sigmaI, RealOpenMM sigmaJ ) const;
    RealOpenMM  cubicMeanSigmaCombiningRule( RealOpenMM sigmaI, RealOpenMM sigmaJ ) const;

    CombiningFunction _combineEpsilons;
    RealOpenMM arithmeticEpsilonCombiningRule( RealOpenMM epsilonI, RealOpenMM epsilonJ ) const;
    RealOpenMM  geometricEpsilonCombiningRule( RealOpenMM epsilonI, RealOpenMM epsilonJ ) const;
    RealOpenMM  harmonicEpsilonCombiningRule(  RealOpenMM epsilonI, RealOpenMM epsilonJ ) const;
    RealOpenMM  hhgEpsilonCombiningRule(       RealOpenMM epsilonI, RealOpenMM epsilonJ ) const;

    /**---------------------------------------------------------------------------------------
    
       Add reduced forces to force vector
    
       @param  particleI            index of particleI
       @param  particleIV           index of particleIV
       @param  reduction            reduction factor
       @param  sign                 +1 or -1 for add/sutracting forces
       @param  force                force vector to add
       @param  forces               force vector for particles
    
       --------------------------------------------------------------------------------------- */
    
    void addReducedForce( unsigned int particleI, unsigned int particleIV,
                          RealOpenMM reduction, RealOpenMM sign,
                          Vec3& force, RealOpenMM** forces ) const;
    
    /**---------------------------------------------------------------------------------------
    
       Calculate pair ixn
    
       @param  combindedSigma       combined sigmas
       @param  combindedEpsilon     combined epsilons
       @param  particleIPosition    particle I position 
       @param  particleJPosition    particle J position 
       @param  force                output force
    
       @return energy for ixn

       --------------------------------------------------------------------------------------- */
    
    RealOpenMM calculatePairIxn( RealOpenMM combindedSigma, RealOpenMM combindedEpsilon,
                                 const Vec3& particleIPosition, const Vec3& particleJPosition,
                                 Vec3& force ) const;

    /**---------------------------------------------------------------------------------------
    
       Calculate Vdw ixns w/ no cutoff
    
       @param numParticles            number of particles
       @param particlePositions       Cartesian coordinates of particles
       @param indexIVs                position index for associated reducing particle
       @param indexClasses            class index for combining sigmas/epsilons (not currently used)
       @param sigmas                  particle sigmas 
       @param epsilons                particle epsilons
       @param reductions              particle reduction factors
       @param vdwExclusions           particle exclusions
       @param forces                  add forces to this vector
    
       @return energy
    
       --------------------------------------------------------------------------------------- */
    
    RealOpenMM calculateNoCutoffForceAndEnergy( int numParticles, RealOpenMM** const particlePositions, 
                                                const std::vector<int>& indexIVs, 
                                                const std::vector<int>& indexClasses, 
                                                const std::vector<RealOpenMM>& sigmas, const std::vector<RealOpenMM>& epsilons,
                                                const std::vector<RealOpenMM>& reductions,
                                                const std::vector< std::vector<int> >& vdwExclusions,
                                                RealOpenMM** forces ) const;
         
};

// ---------------------------------------------------------------------------------------

#endif // _AmoebaReferenceVdwForce___