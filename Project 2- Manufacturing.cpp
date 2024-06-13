#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <array>

enum EventType { ARRIVAL, DEPARTURE, BREAKDOWN, REPAIR, MAINTENANCE, SHIFT_CHANGE, RM_SUPPLY};

class Event {
public:
    double time;
    EventType type;
    int machine_id;
    std::string stage_name;
    int product_type;

    Event(double t, EventType e, int m_id, const std::string& s_name,int pt)
        : time(t), type(e), machine_id(m_id), stage_name(s_name), product_type(pt) {}

};

class Machine {
public:
    int id;
    double processing_time;
    double original_failure_rate;
    double failure_rate;
    double repair_time;
    bool is_operational;
    int breakdown_counter;
    int assigned_operator_id;
    int product_type;
    double last_maintenance_time;


    Machine(int i, double p_time, double f_rate, double r_time, int op_id, int pt)
        : id(i), processing_time(p_time), original_failure_rate(failure_rate),failure_rate(f_rate), repair_time(r_time),
          is_operational(true), breakdown_counter(0), assigned_operator_id(op_id), product_type(pt), last_maintenance_time(0) {}
};


class Operator {
public:
    int id;
    
    std::vector<bool> shift_availability; 

    Operator(int i,const int num_shifts) : id(i) {
        shift_availability.resize(num_shifts,false); 
    }
    
};

class ProductionStage {
public:
    std::string name;
    std::vector<Machine> machines;
    std::vector<Operator> operators;

    ProductionStage() = default;
    ProductionStage(const std::string& n) : name(n) {}


    void add_machine(Machine machine) {
        machines.push_back(machine);
    }

    void add_operator(Operator operator_) {
        operators.push_back(operator_);
    }

    bool is_machine_available(int eventTime,int shift, int pt) {
    for (size_t i = 0; i < machines.size(); ++i) {
        if (i >= operators.size()) {
          
            return false;
        }
        if(eventTime >= machines[i].last_maintenance_time+ 24*6){
                    machines[i].failure_rate = machines[i].original_failure_rate;
        }
   
        if (machines[i].product_type == pt && machines[i].is_operational && operators[i].shift_availability[shift]) {
            return true; 
        }
    }
        return false;
    }

    Machine* get_available_machine(int shift, int pt) {
    for (size_t i = 0; i < machines.size(); ++i) {
        if (i >= operators.size()) {
            
            return nullptr;
        }
        if (machines[i].product_type == pt && machines[i].is_operational && operators[i].shift_availability[shift]) {
            return &machines[i]; 
        }
    }
        return nullptr;
    }
};
class EventComparator {
public:
    bool operator()(const Event& e1, const Event& e2) {
        return e1.time > e2.time;
    }
};

class Simulation {
public:
    std::priority_queue<Event, std::vector<Event>, EventComparator> event_queue;
    std::unordered_map<std::string, ProductionStage> stages;

    int machinecount1;
    double current_time;
    double shift_duration;
    int num_shifts;
    int finished_products_P1;
    int finished_products_P2;
    int end_time;
    int raw_materials_T1;
    int raw_materials_T2;
    int rm_supply_time1;
    int rm_supply_time2;
    int lack_of_rm1;
    int lack_of_rm2;
    int maintenance_interval1;
    int maintenance_interval2;
    
    Simulation(int MachineCount1, double num_shifts) : machinecount1(MachineCount1),current_time(0.0), shift_duration(24/num_shifts), num_shifts(num_shifts),finished_products_P1(0),finished_products_P2(0), end_time(24*365),raw_materials_T1(10000),raw_materials_T2(20000),rm_supply_time1(24*7),rm_supply_time2(24*5),lack_of_rm1(0),lack_of_rm2(0),maintenance_interval1(24*10),maintenance_interval2(24*7) {}

    void add_stage(const std::string& name, const ProductionStage& stage) {
        stages[name] = stage;
    }

    void schedule_event(double time, EventType event_type, int machine_id, const std::string& stage_name,int product_type) {
        event_queue.push(Event(time, event_type, machine_id, stage_name,product_type));
    }

    void initialize() {
        // Raw Material Handling
        ProductionStage raw_material_handling("Raw Material Handling");
        for (int i = 0; i < machinecount1; ++i) {
            Operator op(i, num_shifts);
            raw_material_handling.add_operator(op);
            raw_material_handling.add_machine(Machine(i, 4.0, 0.01, 0.5, op.id,1));
            Operator op2(machinecount1+i, num_shifts);
            raw_material_handling.add_operator(op2);
            raw_material_handling.add_machine(Machine(i, 6.0, 0.025, 0.5, op2.id,2));
        }
        add_stage(raw_material_handling.name, raw_material_handling);

        // Machining
        ProductionStage machining("Machining");
        for (int i = 0; i < machinecount1; ++i) {
            Operator op(i, num_shifts);
            machining.add_operator(op);
            machining.add_machine(Machine(i, 8.0, 0.01, 1, op.id,1));
            Operator op2(machinecount1+machinecount1+i, num_shifts);
            machining.add_operator(op2);
            machining.add_machine(Machine(i, 10.0, 0.03, 1.5, op2.id,2));
        }
        add_stage(machining.name, machining);

        // Assembly
        ProductionStage assembly("Assembly");
        for (int i = 0; i < machinecount1; ++i) {
            Operator op(i, num_shifts);
            assembly.add_operator(op);
            assembly.add_machine(Machine(i, 6, 0.015, 0.75, op.id,1));
            Operator op2(machinecount1+machinecount1+machinecount1+i, num_shifts);
            assembly.add_operator(op2);
            assembly.add_machine(Machine(i, 8, 0.05, 1, op2.id,2));
        }
        add_stage(assembly.name, assembly);

        // Quality Control
        ProductionStage quality_control("Quality Control");
        for (int i = 0; i < machinecount1; ++i) {
            Operator op(i, num_shifts);
            quality_control.add_operator(op);
            quality_control.add_machine(Machine(i, 4, 0.01, 0.75, op.id,1));
            Operator op2(machinecount1+machinecount1+machinecount1+machinecount1+i, num_shifts);
            quality_control.add_operator(op2);
            quality_control.add_machine(Machine(i, 7, 0.02, 1, op2.id,2));
        }
        add_stage(quality_control.name, quality_control);

        // Packaging
        ProductionStage packaging("Packaging");
        for (int i = 0; i < machinecount1; ++i) {
            Operator op(i, num_shifts);
            packaging.add_operator(op);
            packaging.add_machine(Machine(i, 4, 0.005, 0.75, op.id,1));
            Operator op2(machinecount1+machinecount1+machinecount1+machinecount1+machinecount1+i, num_shifts);
            packaging.add_operator(op2);
            packaging.add_machine(Machine(i, 5.5, 0.015, 0.95, op2.id,2));
        }
        add_stage(packaging.name, packaging);

        schedule_event(0.0, ARRIVAL, 1, "Raw Material Handling",1);
        schedule_event(0.0, ARRIVAL, 2, "Raw Material Handling",1);
        schedule_event(0.0, ARRIVAL, 3, "Raw Material Handling",2);
        schedule_event(0.0, ARRIVAL, 3, "Raw Material Handling",2);

        schedule_shift_changes();
        schedule_raw_materials_type1();
        schedule_raw_materials_type2();
        schedule_maintenance();
    }

    void schedule_raw_materials_type1(){
      int supply_time = rm_supply_time1;
      while(supply_time<=end_time){
        schedule_event(supply_time, RM_SUPPLY, -1, "",1);
        supply_time += rm_supply_time1;
      }
    }

    void schedule_raw_materials_type2(){
      int supply_time = rm_supply_time2;
      while(supply_time<=end_time){
        schedule_event(supply_time, RM_SUPPLY, -1, "",2);
        supply_time += rm_supply_time2;
      }
    }

    void schedule_maintenance() {
        for (auto& [stage_name, stage] : stages) {
            for (auto& machine : stage.machines) {
                if(machine.product_type==1){
                    double next_maintenance_time1 = maintenance_interval1;
                    while (next_maintenance_time1 <= end_time) {
                        schedule_event(next_maintenance_time1, MAINTENANCE, machine.id, stage_name, machine.product_type);
                        next_maintenance_time1 += maintenance_interval1;
                    }
                }else{
                    double next_maintenance_time2 = maintenance_interval2;
                    while (next_maintenance_time2 <= end_time) {
                        schedule_event(next_maintenance_time2, MAINTENANCE, machine.id, stage_name, machine.product_type);
                        next_maintenance_time2 += maintenance_interval2;
                    }
                }
            }
        }
    }

    void schedule_shift_changes() {
        for (auto& stage_pair : stages) {
            int shift = 0;
            ProductionStage& stage = stage_pair.second;
            int total_operators=stage.operators.size();
            int operators_per_shift = total_operators/num_shifts; 
            for (auto& operator_ : stage.operators) {
                operator_.shift_availability[shift] = true;
                operators_per_shift--;
                if (operators_per_shift == 0) {
                    operators_per_shift = total_operators/num_shifts;
                    shift +=1;
                    shift = shift % num_shifts;
                }
            }
        }

        double shift_change_time = shift_duration;
        while (shift_change_time <= end_time) {
            schedule_event(shift_change_time, SHIFT_CHANGE, -1, "",-1);
            shift_change_time += shift_duration;
        }
    }

    void run() {
        while (!event_queue.empty() && current_time <= end_time) {
            Event current_event = event_queue.top();
            event_queue.pop();
            current_time = current_event.time;
            if(current_time >= end_time){
              break;
            }

            switch (current_event.type) {
                case ARRIVAL:
                    handle_arrival(current_event);
                    break;
                case DEPARTURE:
                    handle_departure(current_event);
                    break;
                case BREAKDOWN:
                    handle_breakdown(current_event);
                    break;
                case REPAIR:
                    handle_repair(current_event);
                    break;
                case MAINTENANCE:
                    handle_maintenance(current_event);
                    break;
                case SHIFT_CHANGE:
                    handle_shift_change();
                    break;
                case RM_SUPPLY:
                    if(current_event.product_type==1){
                      raw_materials_T1 += 2000;
                    }else if (current_event.product_type==2) {
                      raw_materials_T2 += 3000;
                    }
                    break;
            }
        }
    }

    void handle_arrival(Event& event) {
        ProductionStage& stage = stages[event.stage_name];
        int current_shift = static_cast<int>(current_time / shift_duration) % num_shifts;
        int productType = event.product_type;
        bool isProcessable= true;

        if (stage.is_machine_available(current_time,current_shift,productType)) {
            Machine* machine = stage.get_available_machine(current_shift,productType);
              if (stage.name == "Raw Material Handling") {
                if(productType==1 && raw_materials_T1<=10){
                  isProcessable = false;
                  lack_of_rm1++;
                }else{
                  raw_materials_T1-=10;
                }
                if(productType==2 && raw_materials_T1<=20){
                  isProcessable = false;
                  lack_of_rm2++;
                }else{
                  raw_materials_T2-=20;
                }
              }

              if(isProcessable){
                machine->is_operational = false;
                schedule_event(current_time + machine->processing_time, DEPARTURE, machine->id, event.stage_name,productType);
              }else if(productType==1){
                schedule_event(current_time + rm_supply_time1, ARRIVAL, machine->id, "Raw Material Handling",productType);
              }else if(productType==2){
                schedule_event(current_time + rm_supply_time2, ARRIVAL, machine->id, "Raw Material Handling",productType);
              }
            }else{
                schedule_event(current_time+4, ARRIVAL, -1, event.stage_name,productType);
            }
    }

    void handle_departure(Event& event) {
        ProductionStage& stage = stages[event.stage_name];
        int current_shift = static_cast<int>(current_time / shift_duration) % num_shifts;
        int productType= event.product_type;

        Machine* machine = nullptr;
        for (auto& m : stage.machines) {
            if (m.id == event.machine_id && m.product_type==productType) {
                machine = &m;
                break;
            }
        }
        if (machine) {
            machine->is_operational = true;

            if (event.stage_name == "Packaging") {
                if(productType==1){
                  finished_products_P1++;
                } 
                else if(productType==2){
                  finished_products_P2++;
                } 
            }
            double random_number = static_cast<double>(rand()) / RAND_MAX;
            if (random_number < machine->failure_rate) {
                // Schedule a breakdown event
                schedule_event(current_time, BREAKDOWN, machine->id, event.stage_name,productType);
            }
            
            Machine* mac= stages["Raw Material Handling"].get_available_machine(current_shift,1);
            if(mac && raw_materials_T1>= 10){
              schedule_event(current_time+2, ARRIVAL, mac->id, "Raw Material Handling",1);
            }
            Machine* mac2= stages["Raw Material Handling"].get_available_machine(current_shift,2);
            if(mac2 && raw_materials_T2>= 20){
              schedule_event(current_time+2, ARRIVAL, mac2->id, "Raw Material Handling",2);
            }

            // Schedule the next arrival event
            std::string next_stage_name = get_next_stage(event.stage_name);
            if (next_stage_name!="") {
              schedule_event(current_time, ARRIVAL, event.machine_id, next_stage_name,productType);
                
            }
        }
    }

    void handle_breakdown(const Event& event) {
        ProductionStage& stage = stages[event.stage_name];
        Machine* machine = nullptr;
        for (auto& m : stage.machines) {
            if (m.id == event.machine_id && m.product_type==event.product_type) {
                machine = &m;
                break;
            }
        }
        if (machine) {
            machine->breakdown_counter++;
            machine->is_operational = false;
            schedule_event(current_time + machine->repair_time, REPAIR, machine->id, event.stage_name,event.product_type);
        }
    }

    void handle_repair(const Event& event) {
        ProductionStage& stage = stages[event.stage_name];
        Machine* machine = nullptr;
        for (auto& m : stage.machines) {
            if (m.id == event.machine_id && m.product_type== event.product_type) {
                machine = &m;
                break;
            }
        }
        if (machine) {
            machine->is_operational = true;
        }
    }

    void handle_maintenance(const Event& event) {
        auto& stage = stages[event.stage_name];
        auto& machine = stage.machines[event.machine_id];
        machine.is_operational = false;  
        machine.failure_rate *= 0.5; //reduce failure rate
        machine.last_maintenance_time = current_time;  
        schedule_event(current_time + machine.repair_time, REPAIR, machine.id, event.stage_name, event.product_type);
    }

    void handle_shift_change() {
        int current_shift = static_cast<int>(current_time / shift_duration) % num_shifts;
        int next_shift = (current_shift + 1) % num_shifts;

        for (auto& stage_pair : stages) {
            ProductionStage& stage = stage_pair.second;
            for (auto& operator_ : stage.operators) {
                // If operator was working in the current shift, mark them as unavailable for the next shift
                if (operator_.shift_availability[current_shift]) {
                    operator_.shift_availability[next_shift] = false;
                } else {
                    operator_.shift_availability[next_shift] = true;
                }
            }
        }
    }

    std::string get_next_stage(const std::string& current_stage_name) {
        std::vector<std::string> stages_order = {
            "Raw Material Handling", "Machining", "Assembly", "Quality Control", "Packaging"
        };

        auto it = std::find(stages_order.begin(), stages_order.end(), current_stage_name);
        if (it != stages_order.end() && (it + 1) != stages_order.end()) {
            return *(it + 1);
        }
        return "";
    }
};

void suggest_improvements(const Simulation& sim) {
    bool flag=false;
    std::cout << "Recommendations:" << std::endl;
    std::cout << "-----------------" << std::endl;

    // Raw Material Supply Adjustment recommendation
    int total_lack_of_rm = sim.lack_of_rm1 + sim.lack_of_rm2;
    if(total_lack_of_rm>0){
        std::cout << "1. Raw Material Supply Adjustment:" << std::endl;
    }
    if (sim.lack_of_rm1 > 0) {
        std::cout << "   Raw material shortage incidents occurred " << sim.lack_of_rm1 << " times for Product Type 1." << std::endl;        
        std::cout << " Suggestion: Increase the frequency or quantity of raw material supplies of product 1 to minimize delays due to resource shortages or reduce the machine count." << std::endl;
    }
    if(sim.lack_of_rm2>0){
        std::cout << "   Raw material shortage incidents occurred " << sim.lack_of_rm2 << " times for Product Type 2." << std::endl; 
        std::cout << " Suggestion: Increase the frequency or quantity of raw material supplies of product 2 to minimize delays due to resource shortages or reduce the machine count." << std::endl;
        std::cout<< "---------------------------------"<<std::endl;
    }

    flag=false;
    // Shift Management recommendation
    for (const auto& stage_pair : sim.stages) {
        const auto& stage_name = stage_pair.first;
        const auto& stage = stage_pair.second;
        int total_idle_time = 0;

        for (int shift = 0; shift < sim.num_shifts; ++shift) {
                int total_idle_time = 0;
                for (size_t i = 0; i < stage.machines.size(); ++i) {
                    if (i >= stage.operators.size()) {
                        continue;
                    }
                    if (stage.machines[i].product_type == 1 && stage.operators[i].shift_availability[shift]) {
                        total_idle_time += stage.machines[i].processing_time - stage.machines[i].processing_time * (1 - stage.machines[i].failure_rate);
                    }
                }
                if (total_idle_time > 0) {
                    std::cout << "Shift " << shift + 1 << " in Stage " << stage_name << " (Product Type 1): " << total_idle_time << " units of idle time." << std::endl;
                    flag=true;
                }
                
                total_idle_time = 0;
                for (size_t i = 0; i < stage.machines.size(); ++i) {
                    if (i >= stage.operators.size()) {
                        continue;
                    }
                    if (stage.machines[i].product_type == 2 && stage.operators[i].shift_availability[shift]) {
                        total_idle_time += stage.machines[i].processing_time - stage.machines[i].processing_time * (1 - stage.machines[i].failure_rate);
                    }
                }
                if (total_idle_time > 0) {
                    flag=true;
                    std::cout << "Shift " << shift + 1 << " in Stage " << stage_name << " (Product Type 2): " << total_idle_time << " units of idle time." << std::endl;
                }
            }
    }
    if(flag){
        std::cout << " Suggestion: Optimize shift schedules to ensure operators are available when needed, reducing idle time for machines." << std::endl;
        std::cout<< "---------------------------------"<<std::endl;
    }
    
    // Stage Optimization recommendation
    flag=false;
    for (const auto& stage_pair : sim.stages) {
        const auto& stage_name = stage_pair.first;
        const auto& stage = stage_pair.second;
        double max_waiting_time = 0.0;
        for (const auto& machine : stage.machines) {
            // Calculate the maximum waiting time for machines in the stage
            double waiting_time = machine.processing_time * (1 - machine.failure_rate);
            if (waiting_time > max_waiting_time) {
                max_waiting_time = waiting_time;
            }
        }
        if (max_waiting_time > 0.0) {
            flag= true;
            std::cout << "3. Stage Optimization for Stage " << stage_name << ":" << std::endl;
            std::cout << "   Machines in this stage have a maximum waiting time of " << max_waiting_time << " units." << std::endl;
        }
    }
    if(flag){
        std::cout << "Suggestion: Analyze stages with high waiting times or low throughput and identify potential improvements, such as adding more machines or optimizing machine assignment logic." << std::endl;
        std::cout<< "---------------------------------"<<std::endl;

    }

    // Breakdown rates recommendation
    flag= false;
    for (const auto& stage_pair : sim.stages) {
        const auto& stage_name = stage_pair.first;
        const auto& stage = stage_pair.second;
        int total_breakdowns = 0;
        for (const auto& machine : stage.machines) {
            total_breakdowns += machine.breakdown_counter;
        }
        if(total_breakdowns>0){
            std::cout << "4. Breakdowns for Stage " << stage_name << ":" << std::endl;
            std::cout << "   Machines in this stage had " << total_breakdowns << " breakdowns." << std::endl;
            flag=true;
        }
    }
    if(flag){
        std::cout << "Suggestion: Reduce the maintenance interval of machines to prevent malfunctions." << std::endl;
    }
    
}

int main() {

    int MachineCount1;
    int ShiftNum;
    std::cout << "Enter number of machines: ";
    std::cin >> MachineCount1;
    std::cout << "Enter number of shifts per day: ";
    std::cin >> ShiftNum;

    Simulation sim(MachineCount1,ShiftNum);
    sim.initialize();
    sim.run();

    std::cout << "Finished products of product type 1: " << sim.finished_products_P1 << std::endl;
    std::cout << "Finished products of product type 2: " << sim.finished_products_P2 << std::endl;
    std::cout << "Lack of Raw Materials of product type 1: " << sim.lack_of_rm1 << std::endl;
    std::cout << "Lack of Raw Materials of product type 2: " << sim.lack_of_rm2 << std::endl;
    std::cout << std::endl;

    suggest_improvements(sim);

    return 0;
}
