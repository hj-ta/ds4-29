#include "BpTree.h"


// 데이터 삽입
bool BpTree::Insert(FlightData* newData) {
    if (!root) {
        root = new BpTreeDataNode();
        root->insertDataMap(newData->GetAirlineName(), newData);
        return true;
    }

    BpTreeNode* targetNode = searchDataNode(newData->GetAirlineName());
    targetNode->insertDataMap(newData->GetAirlineName(), newData);

    while (excessDataNode(targetNode)) {
        splitDataNode(targetNode);
        targetNode = targetNode->getParent();
    }
    return true;
}

// 데이터 노드 초과 여부 확인
bool BpTree::excessDataNode(BpTreeNode* pDataNode) {
    return pDataNode->getDataMap()->size() >= order;
}

/////////////이거 구현 마저 ㄱ
bool BpTree::excessIndexNode(BpTreeNode* pIndexNode) {
    if (pIndexNode->getIndexMap()->size() > order - 1)return true;//order is equal to the number of elements 
    else return false;
}


//helper function
string getFirstKey(BpTreeNode* node) {
    if (node->getDataMap() && !node->getDataMap()->empty()) {
        return node->getDataMap()->begin()->first;
    }
    if (node->getIndexMap() && !node->getIndexMap()->empty()) {
        return node->getIndexMap()->begin()->first;
    }
    return "";
}





// 데이터 노드 분할
void BpTree::splitDataNode(BpTreeNode* pDataNode) {
    BpTreeNode* rightSplit = new BpTreeDataNode();

    auto curDataMap = pDataNode->getDataMap();
    auto splitIter = next(curDataMap->begin(), curDataMap->size() / 2);

    // 오른쪽 데이터 노드로 데이터 이동
    for (auto iter = splitIter; iter != curDataMap->end();) {
        rightSplit->insertDataMap(iter->first, iter->second);
        iter = curDataMap->erase(iter);
    }

    // 부모 노드가 없으면 새로운 인덱스 노드 생성
    if (!pDataNode->getParent()) {
        BpTreeNode* newParent = new BpTreeIndexNode();
        root = newParent;
        pDataNode->setParent(newParent);
        newParent->setMostLeftChild(pDataNode);
    }

    


    // 부모 노드에 새 키 삽입
    string firstKey = getFirstKey(rightSplit);
    rightSplit->setParent(pDataNode->getParent());
    pDataNode->getParent()->insertIndexMap(firstKey, rightSplit);

    // next, prev 포인터 설정
    rightSplit->setNext(pDataNode->getNext());
    if (pDataNode->getNext()) {
        pDataNode->getNext()->setPrev(rightSplit);
    }
    pDataNode->setNext(rightSplit);
    rightSplit->setPrev(pDataNode);
}

// 인덱스 노드 분할
void BpTree::splitIndexNode(BpTreeNode* pIndexNode) {
    BpTreeNode* rightSplit = new BpTreeIndexNode();

    auto curIndexMap = pIndexNode->getIndexMap();
    auto splitIter = next(curIndexMap->begin(), curIndexMap->size() / 2);
    string middleKey = splitIter->first; // 중간 키를 부모로 올림
    ++splitIter;

    // 오른쪽 인덱스 노드로 데이터 이동
    for (auto iter = splitIter; iter != curIndexMap->end();) {
        rightSplit->insertIndexMap(iter->first, iter->second);
        iter->second->setParent(rightSplit); // 부모 노드 설정
        iter = curIndexMap->erase(iter);
    }

    // 부모 노드가 없으면 새로운 인덱스 노드 생성
    if (!pIndexNode->getParent()) {
        BpTreeNode* newParent = new BpTreeIndexNode();
        root = newParent;
        pIndexNode->setParent(newParent);
        newParent->setMostLeftChild(pIndexNode);
    }

    // 부모 노드에 중간 키 삽입
    rightSplit->setParent(pIndexNode->getParent());
    pIndexNode->getParent()->insertIndexMap(middleKey, rightSplit);
}

// 데이터 노드 검색
BpTreeNode* BpTree::searchDataNode(string n) {
    BpTreeNode* pCur = root;

    while (!pCur->isLeafNode()) {
        auto curIndexMap = pCur->getIndexMap();
        auto iter = curIndexMap->lower_bound(n);
        pCur = (iter == curIndexMap->end()) ? prev(curIndexMap->end())->second : iter->second;
    }
    return pCur;
}

// 트리 출력
void BpTree::Print() {
    BpTreeNode* cur = root;

    while (!cur->isLeafNode()) {
        cur = cur->getMostLeftChild();
    }

    while (cur) {
        auto curDataMap = cur->getDataMap();
        for (const auto& [key, value] : *curDataMap) {
            *fout << key << ": " << value->GetAirlineName() << " | "
                << value->GetFlightNumber() << " | " << value->GetDestination() << " | "
                << value->GetNumberofSeats() << " | " << value->GetStatus() << endl;
        }
        cur = cur->getNext();
    }
}

//없으면 오류나니까

bool BpTree::SearchRange(string start, string end) {
    BpTreeNode* cur = root;

    // 가장 왼쪽 리프 노드로 이동
    while (cur && !cur->isLeafNode()) {
        cur = cur->getMostLeftChild();
    }

    bool found = false;
    // 리프 노드 순회
    while (cur) {
        auto curDataMap = cur->getDataMap();
        for (const auto& [key, value] : *curDataMap) {
            if (key >= start && key <= end) {
                cout << key << ": " << value->GetAirlineName() << endl;
                found = true;
            }
        }
        if (cur->getNext() && cur->getNext()->getDataMap()->begin()->first > end) {
            break;
        }
        cur = cur->getNext();
    }

    return found;
}

bool BpTree::SearchModel(string model_name) {
    BpTreeNode* cur = root;

    // 가장 왼쪽 리프 노드로 이동
    while (!cur->isLeafNode()) {
        cur = cur->getMostLeftChild();
    }

    // 리프 노드 순회
    while (cur) {
        auto dataMap = cur->getDataMap();
        if (dataMap->find(model_name) != dataMap->end()) {
            std::cout << model_name << ": Found" << std::endl;
            return true;
        }
        cur = cur->getNext();
    }
    return false;
}