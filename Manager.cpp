#include "Manager.h"
#include <sstream>
#include <algorithm>

using namespace std;

// ������
Manager::Manager(int bpOrder) {
    avl = new AVLTree();
    fout = new ofstream("log.txt", ofstream::app); // �α� ������ ���� ofstream ��ü
    bp = new BpTree(fout, bpOrder);
}

// �Ҹ���
Manager::~Manager() {
    delete avl;
    delete bp;
    if (fout) {
        fout->close(); // ��Ʈ�� �ݱ�
        delete fout;   // �޸� ����
    }
}

// ��ɾ� ����
void Manager::run(const char* command_txt) {
    ifstream fin(command_txt);
    if (!fin.is_open()) {
        printErrorCode(0);
        return;
    }

    string line;

    while (getline(fin, line)) {
        size_t delimiterPos = line.find(" | ");
        string command = line.substr(0, delimiterPos);
        string params = (delimiterPos != string::npos) ? line.substr(delimiterPos + 3) : "";

        if (command == "LOAD") {
            if (!LOAD()) printErrorCode(1);
        }
        else if (command == "VLOAD") {
            if (!VLOAD()) printErrorCode(2);
        }
        else if (command == "ADD") {
            if (!ADD()) printErrorCode(3);
        }
        else if (command == "PRINT_BP") {
            if (!PRINT_BP()) printErrorCode(4);
        }
        else if (command == "SEARCH_BP") {
            if (params.find(" | ") == string::npos) {
                if (!SEARCH_BP(params)) printErrorCode(5);
            }
            else {
                size_t splitPos = params.find(" | ");
                string start = params.substr(0, splitPos);
                string end = params.substr(splitPos + 3);
                if (!SEARCH_BP(start, end)) printErrorCode(6);
            }
        }
        else if (command == "SEARCH_AVL") {
            if (!SEARCH_AVL(params)) printErrorCode(7);
        }
        else if (command == "VPRINT") {
            if (!VPRINT(params)) printErrorCode(8);
        }
        else {
            printErrorCode(0); // �� �� ���� ��ɾ�
        }
    }

    fin.close();
}

// ���ڿ� ���� ����
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

// ������ �ε�
bool Manager::LOAD() {
    ifstream fin("input_data.txt");
    if (!fin.is_open()) return false;

    string line;
    while (getline(fin, line)) {
        stringstream ss(line);
        string airlineName, flightNumber, destination, seatsStr, status;

        getline(ss, airlineName, '|');
        getline(ss, flightNumber, '|');
        getline(ss, destination, '|');
        getline(ss, seatsStr, '|');
        getline(ss, status, '|');

        FlightData* newData = new FlightData();
        newData->SetAirlineName(trim(airlineName));
        newData->SetFlightNumber(trim(flightNumber));
        newData->SetDestination(trim(destination));

        string seatsTrimmed = trim(seatsStr);
        if (!seatsTrimmed.empty() && all_of(seatsTrimmed.begin(), seatsTrimmed.end(), ::isdigit)) {
            newData->SetNumberofSeats(stoi(seatsTrimmed));
        }
        else {
            newData->SetNumberofSeats(0); // �⺻������ ó��
        }

        newData->SetStatus(trim(status));
        avl->Insert(newData);
        bp->Insert(newData);
    }

    fin.close();
    return true;
}


// AVL Ʈ�� ������ �ε�
bool Manager::VLOAD() {
    avl->GetVector(Print_vector);
    sort(Print_vector.begin(), Print_vector.end(), [](FlightData* a, FlightData* b) {
        return a->GetAirlineName() < b->GetAirlineName();
        });
    return true;
}

// ������ �߰�
bool Manager::ADD() {
    string airlineName = "KoreanAir"; // ���� ������
    string flightNumber = "KE123";
    string destination = "Seoul";
    int seats = 7;
    string status = "On Time";

    if ((airlineName == "KoreanAir" || airlineName == "ASIANA") && seats > 7) {
        printErrorCode(8);
        return false;
    }
    if ((airlineName == "JEJU" || airlineName == "JeanAir") && seats > 5) {
        printErrorCode(8);
        return false;
    }
    if (airlineName == "AirKwangwoon" && seats > 6) {
        printErrorCode(8);
        return false;
    }

    if (SEARCH_BP(airlineName)) {
        BpTreeNode* node = bp->searchDataNode(airlineName);
        auto it = node->getDataMap()->find(airlineName);
        if (it != node->getDataMap()->end() && status == "On Time") {
            it->second->SetSeatsDec();
            return true;
        }
    }

    FlightData* newData = new FlightData();
    newData->SetAirlineName(airlineName);
    newData->SetFlightNumber(flightNumber);
    newData->SetDestination(destination);
    newData->SetNumberofSeats(seats);
    newData->SetStatus(status);

    avl->Insert(newData);
    bp->Insert(newData);
    return true;
}

// B+ Ʈ�� ���
bool Manager::PRINT_BP() {
    BpTreeNode* cur = bp->getRoot(); // ��Ʈ ��带 �����ɴϴ�.

    // ��Ʈ ��忡�� ���� ���� ���� ���� �̵�
    while (cur && !cur->isLeafNode()) {
        cur = cur->getMostLeftChild();
    }

    // ���� ��� ��ȸ
    while (cur) {
        auto curDataMap = cur->getDataMap(); // getDataMap()�� std::map<string, FlightData*>* ��ȯ
        if (curDataMap) { // ������ ���� ��ȿ���� Ȯ��
            for (const auto& entry : *curDataMap) {
                FlightData* value = entry.second; // value�� FlightData*�� ����
                cout << value->GetAirlineName() << " | "
                    << value->GetFlightNumber() << " | "
                    << value->GetDestination() << " | "
                    << value->GetNumberofSeats() << " | "
                    << value->GetStatus() << endl;
            }
        }
        cur = cur->getNext(); // ���� ���� ���� �̵�
    }

    return true;
}



// B+ Ʈ�� �˻�
bool Manager::SEARCH_BP(string name) {
    BpTreeNode* resultNode = bp->searchDataNode(name);
    if (resultNode) {
        auto it = resultNode->getDataMap()->find(name);
        if (it != resultNode->getDataMap()->end()) {
            cout << it->second->GetAirlineName() << " | "
                << it->second->GetFlightNumber() << " | "
                << it->second->GetDestination() << " | "
                << it->second->GetNumberofSeats() << " | "
                << it->second->GetStatus() << endl;
            return true;
        }
    }
    return false;
}

// B+ Ʈ�� ���� �˻�
bool Manager::SEARCH_BP(string start, string end) {
    return bp->SearchRange(start, end);
}

// AVL Ʈ�� �˻�
bool Manager::SEARCH_AVL(string name) {
    FlightData* result = avl->Search(name);
    if (result) {
        cout << result->GetAirlineName() << " | "
            << result->GetFlightNumber() << " | "
            << result->GetDestination() << " | "
            << result->GetNumberofSeats() << " | "
            << result->GetStatus() << endl;
        return true;
    }
    return false;
}

// VPRINT ����
bool Manager::VPRINT(string type_) {
    if (type_ == "AVL") {
        for (const auto& data : Print_vector) {
            cout << data->GetAirlineName() << " | "
                << data->GetFlightNumber() << " | "
                << data->GetDestination() << " | "
                << data->GetNumberofSeats() << " | "
                << data->GetStatus() << endl;
        }
        return true;
    }
    return false;
}

// ���� �ڵ� ���
void Manager::printErrorCode(int n) {
    if (fout && fout->is_open()) {
        *fout << "========== ERROR ==========" << endl;
        *fout << n << endl;
        *fout << "===========================" << endl;
    }
}