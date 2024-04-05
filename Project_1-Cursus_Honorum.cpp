#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>

using namespace std;

struct Politician {
    int age;
    string office;
    int yearsOfService;
    int lifeExpantancy;
    bool canBeElected;

    Politician(int a, string o, int y, int l, bool e) : age(a), office(o), yearsOfService(y), lifeExpantancy(l), canBeElected(e) {}
};


int lifeExpectancy(default_random_engine& generator) {
    const int mean = 55;  
    const int stddev = 10;  
    const int A = 25;  
    const int B = 80; 
    normal_distribution<double> ageDistribution(mean, stddev);
    int age;
    do {
        age = ageDistribution(generator);
    } while (age < A || age > B);  
    return age;
}

void initializePoliticians(vector<Politician>& Quaestors, vector<Politician>& Aediles,vector<Politician>&Praetors,vector<Politician>& Consuls, int quaestors, int aediles, int praetors, int consuls) {
    default_random_engine generator;

    //This initial politicians served for 1 year because the simulation year was started from the 2nd year. At the beginning of the year, their ages increased.
    for (int i = 0; i < quaestors; ++i) {
        Quaestors.push_back(Politician(30, "Quaestor",1,lifeExpectancy(generator),true));
    }
    for (int i = 0; i < aediles; ++i) {
        Aediles.push_back(Politician(36, "Aedile",1,lifeExpectancy(generator),true));
    }
    for (int i = 0; i < praetors; ++i) {
        Praetors.push_back(Politician(39, "Praetor",1,lifeExpectancy(generator),true));
    }
    for (int i = 0; i < consuls; ++i) {
        Consuls.push_back(Politician(42, "Consul",1,lifeExpectancy(generator),false));
    }
}

//To get random index of the candidates
int getRandomValidCandidateIndex(vector<Politician> candidates) {
    int randomIndex=candidates.size();
    if(candidates.size()==1){
        randomIndex =0; 
    }else{
        do {
            randomIndex = rand() % candidates.size();
        } while (candidates[randomIndex].age > candidates[randomIndex].lifeExpantancy);
    }
    return randomIndex; 
}

void calculateAgeDistribution(string office,vector<int> ages) {

    sort(ages.begin(), ages.end());

    int totalPoliticians = ages.size();
    int minAge = ages.front();
    int maxAge = ages.back();
    int meanAge = accumulate(ages.begin(), ages.end(), 0) / (totalPoliticians);

    cout << office << " Age Distribution Summary:" << endl;
    cout << "Minimum Age: " << minAge << endl;
    cout << "Maximum Age: " << maxAge << endl;
    cout << "Average Age: " << meanAge << endl;

}

//Candidates determined for rank progression, and age advancement in the pool.
void progression(vector<Politician>& politicians, vector<Politician>& candidatesFromPool, vector<Politician>& candidates){

    if(politicians[0].office=="Consul"){
       while (politicians.size()>0) {
            Politician politician = politicians.back();
            politician.age += 1;
            politician.yearsOfService++; //Function of the consuls' 'yearsOfService' is different from other offices. Where 'yearsOfService' represents the year progress after serving in the year in which the consul was elected. So, it is advanced every year(It is reset when consul elected again).

            if ( politician.age > politician.lifeExpantancy) { //remove from pool
                politicians.pop_back();                
                continue;
            }
            if(politician.yearsOfService >= 10){ //10 years after being elected, the consul can be re-elect.
              politician.canBeElected = true;
            }
            candidatesFromPool.push_back(politician); //All consuls that have not expired and are in the consul pool become candidates again.
            politicians.pop_back();
       }
    }else if(politicians[0].office=="Quaestor"){
       while (politicians.size()>0) {
            Politician politician = politicians.back();
            politician.age += 1;

            if ( politician.age > politician.lifeExpantancy) {
                politicians.pop_back();
                continue;               
            }
            if(politician.age >= 36 && politician.yearsOfService >= 2) {
                candidates.push_back(politician); //Questors who meet the conditions become candidates for aedile.
            }else{
                candidatesFromPool.push_back(politician); //Those who do not meet the conditions become candidates for questor again.
            }
            politicians.pop_back();
       }
    }else if(politicians[0].office=="Aedile"){ //same as Questors
       while (politicians.size()>0) {
            Politician politician = politicians.back();
            politician.age += 1;

            if ( politician.age > politician.lifeExpantancy) {
                politicians.pop_back();                
                continue;
            }
            if (politician.age >= 39 && politician.yearsOfService >= 2) {
                candidates.push_back(politician);           
            }else{

                candidatesFromPool.push_back(politician);

            }
            politicians.pop_back();
        }
    }else if(politicians[0].office=="Praetor"){ //same as Questors
       while (politicians.size()>0) {
            Politician politician = politicians.back();
            politician.age += 1;

            if ( politician.age > politician.lifeExpantancy) {
                politicians.pop_back();                
                continue;
            }
            if (politician.age >= 42 && politician.yearsOfService >= 2) {
                candidates.push_back(politician);           
            }else{
                candidatesFromPool.push_back(politician);
            }
            politicians.pop_back();
        }
    }
}

void simulate(int totalYears, int quaestors, int aediles, int praetors, int consuls,int newCandidatesMean, int newCandidatesStd, int startingPSI,int unfilledPositionPenalty, int reelectionPenalty) {

    //Pools of politicians
    vector<Politician> Quaestors; 
    vector<Politician> Aediles;
    vector<Politician> Praetors;
    vector<Politician> Consuls;

    //Candidates of each position
    vector<Politician> quaestorCandidates;
    vector<Politician> aedileCandidates;
    vector<Politician> praetorCandidates;
    vector<Politician> consulCandidates;

    initializePoliticians(Quaestors,Aediles,Praetors,Consuls, quaestors, aediles, praetors, consuls);
    default_random_engine generator;
    normal_distribution<double> newCandidatesDistribution(newCandidatesMean, newCandidatesStd);

    //The ages of all politicians who have served are kept
    vector<int> quaestorAges;
    vector<int> aedileAges;
    vector<int> praetorAges;
    vector<int> consulAges;

    int psi = startingPSI;
    double totalQuaestorsFilled = 20;
    double totalAedilesFilled = 10;
    double totalPraetorsFilled = 8;
    double totalConsulsFilled = 2;

    for (int year = 2; year <= totalYears; ++year) {

        int consulPenalty = 0; //To calculate the re-electing of a consul's penalty

        int availableQuaestors = quaestors;
        int availableAediles = aediles;
        int availablePraetors = praetors;
        int availableConsuls = consuls;

        int newCandidates = max(0, static_cast<int>(round(newCandidatesDistribution(generator))));

        //age progression and office candidates 
        progression(Consuls,consulCandidates,consulCandidates);
        progression(Praetors,praetorCandidates,consulCandidates);
        progression(Aediles,aedileCandidates,praetorCandidates);
        progression(Quaestors,quaestorCandidates,aedileCandidates);

        // Influx of new candidates
        for (int i = 0; i < newCandidates; ++i) {
            int l = lifeExpectancy(generator);
            while(lifeExpectancy(generator)<=30){
                l = lifeExpectancy(generator);
            }
            quaestorCandidates.push_back(Politician(30, "Candidate",0,l,true));
        }

        // Elect candidates for each position
        //consuls
        vector<Politician> validConsulCandidates; //To store consuls who can be elected.
        if(availableConsuls > 0 && !consulCandidates.empty()) {
            for(auto it = consulCandidates.begin(); it != consulCandidates.end();){ //Candidates who can be elected as consul are transferred to 'validConsulCandidates'.
                auto& candidate = *it;
                if(candidate.canBeElected){
                    validConsulCandidates.push_back(candidate);
                    it = consulCandidates.erase(it);
                    continue;
                }
                it++;
            }
            while (availableConsuls > 0 && (!consulCandidates.empty() || !validConsulCandidates.empty())) {
            //If electable candidates are available, they are elected as consuls first. If not, it is re-elected from among those who have not completed 10 years.
                if(!validConsulCandidates.empty()){
                    int index = getRandomValidCandidateIndex(validConsulCandidates);
                    Politician candidate = validConsulCandidates[index];
                    candidate.yearsOfService=0;
                    candidate.office="Consul";
                    candidate.canBeElected =false;
                    consulAges.push_back(candidate.age);
                    Consuls.push_back(candidate);
                    availableConsuls--;
                    validConsulCandidates.erase(validConsulCandidates.begin()+index);
                }else if(!consulCandidates.empty()){ //false ones
                    int index = getRandomValidCandidateIndex(consulCandidates);
                    Politician candidate = consulCandidates[index];
                    candidate.yearsOfService=0;
                    consulAges.push_back(candidate.age);
                    Consuls.push_back(candidate);
                    availableConsuls--;
                    psi -= 10; //Update psi with re-electing penalty
                    consulCandidates.erase(consulCandidates.begin()+index); 
                }
            }
        }
        // After the elections are over, unelected consul candidates return to their pools.
        if(!consulCandidates.empty() && availableConsuls==0){
            for(int i=0;i<consulCandidates.size();i++){
                Politician candidate = consulCandidates.back();
                Consuls.push_back(candidate);
                consulCandidates.pop_back();
            }
        }
        if(!validConsulCandidates.empty() && availableConsuls==0 ){
            for(int i=0;i<validConsulCandidates.size();i++){
                Politician candidate = validConsulCandidates.back();
                if(candidate.office=="Praetor"){
                    Praetors.push_back(candidate);
                    validConsulCandidates.pop_back();
                }else{
                    Consuls.push_back(candidate);
                    validConsulCandidates.pop_back();
                }
            }
        }

        //Praetors
        while (availablePraetors > 0 && !praetorCandidates.empty()) {
            int index =getRandomValidCandidateIndex(praetorCandidates); 
            Politician candidate = praetorCandidates[index];
            if(candidate.office=="Aedile"){ //If the random politician is from Aediles, years of service as praetor become 0.
                candidate.yearsOfService=0;
                candidate.office="Praetor";
            }
            candidate.yearsOfService++; //It was elected and served for one year
            praetorAges.push_back(candidate.age);
            Praetors.push_back(candidate); //Then, return to the pool.
            availablePraetors--;
            praetorCandidates.erase(praetorCandidates.begin()+index);
        }
        if(!praetorCandidates.empty() && availablePraetors==0){
        //Those who are not elected, after the elections are over, those in the aedile position return to their own pools, and those who are praetors return to their own pools.
            for(int i=0;i<praetorCandidates.size();i++){
                Politician candidate =praetorCandidates.back();
                if(candidate.office=="Aedile"){
                    Aediles.push_back(candidate);
                }else{
                    Praetors.push_back(candidate);
                }
                praetorCandidates.pop_back();
            }
        }

        //Aediles
        while (availableAediles > 0 && !aedileCandidates.empty()) { //Same as Praetors.
            int index = getRandomValidCandidateIndex(aedileCandidates);
            Politician candidate = aedileCandidates[index];
            if(candidate.office=="Quaestor"){
                candidate.yearsOfService=0;
                candidate.office="Aedile";
            }
            candidate.yearsOfService++;
            aedileAges.push_back(candidate.age);
            Aediles.push_back(candidate);
            availableAediles--;
            aedileCandidates.erase(aedileCandidates.begin()+index);
        }
        if(!aedileCandidates.empty() && availableAediles==0){
            for(int i=0;i<aedileCandidates.size();i++){
                Politician candidate =aedileCandidates.back();
                if(candidate.office=="Quaestor"){
                    Quaestors.push_back(candidate);
                }else{
                    Aediles.push_back(candidate);
                }
                aedileCandidates.pop_back();
            }
        }
        
        //Quaestors
        while(availableQuaestors > 0 && !quaestorCandidates.empty()) { //Same as Praetors.
            int index = getRandomValidCandidateIndex(quaestorCandidates);
            Politician candidate = quaestorCandidates[index];
            candidate.yearsOfService++;
            candidate.office="Quaestor";
            quaestorAges.push_back(candidate.age);
            Quaestors.push_back(candidate);
            availableQuaestors--;
            quaestorCandidates.erase(quaestorCandidates.begin()+index);
        }
        if(!quaestorCandidates.empty() && availableQuaestors==0){
            while(quaestorCandidates.size()>0){
                Politician candidate =quaestorCandidates.back();
                if(candidate.office=="Quaestor"){
                    Quaestors.push_back(candidate);
                }
                //Unselected new candidates do not remain in the pool.
                quaestorCandidates.pop_back();
            }
        }

        validConsulCandidates.clear();

        // Update PSI
        int unfilledPositions = availableAediles+availableConsuls+availableQuaestors+availablePraetors;
        psi -= (unfilledPositionPenalty * unfilledPositions);
    
        //Calculate annual fill rates
        totalQuaestorsFilled += (quaestors-availableQuaestors);
        totalAedilesFilled += (aediles-availableAediles);
        totalPraetorsFilled += (praetors-availablePraetors);
        totalConsulsFilled += (consuls-availableConsuls);
    }

    double annualFillRateQuaestor = totalQuaestorsFilled / (totalYears);
    double annualFillRateAedile = totalAedilesFilled / (totalYears);
    double annualFillRatePraetor = totalPraetorsFilled / (totalYears);
    double annualFillRateConsul = totalConsulsFilled / (totalYears);


    cout << "End-of-Simulation PSI: " << psi << endl;
    cout << "Overall Annual Fill Rate:" << endl;
    cout << "Quaestor: " << annualFillRateQuaestor << "%" << endl;
    cout << "Aedile: " << annualFillRateAedile<< "%" << endl;
    cout << "Praetor: " << annualFillRatePraetor << "%" << endl;
    cout << "Consul: " << annualFillRateConsul << "%" << endl;

    calculateAgeDistribution("Quaestors",quaestorAges);
    calculateAgeDistribution("Aediles",aedileAges);
    calculateAgeDistribution("Praetors",praetorAges);
    calculateAgeDistribution("Consuls",consulAges);

}

int main() {
    simulate(200, 20, 10, 8, 2, 15, 5, 100, 5, 10);
    return 0;
}

