#include "BpTree.h"


// ������ ����
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

// ������ ��� �ʰ� ���� Ȯ��
bool BpTree::excessDataNode(BpTreeNode* pDataNode) {
    return pDataNode->getDataMap()->size() >= order;
}

/////////////�̰� ���� ���� ��
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





// ������ ��� ����
void BpTree::splitDataNode(BpTreeNode* pDataNode) {
    BpTreeNode* rightSplit = new BpTreeDataNode();

    auto curDataMap = pDataNode->getDataMap();
    auto splitIter = next(curDataMap->begin(), curDataMap->size() / 2);

    // ������ ������ ���� ������ �̵�
    for (auto iter = splitIter; iter != curDataMap->end();) {
        rightSplit->insertDataMap(iter->first, iter->second);
        iter = curDataMap->erase(iter);
    }

    // �θ� ��尡 ������ ���ο� �ε��� ��� ����
    if (!pDataNode->getParent()) {
        BpTreeNode* newParent = new BpTreeIndexNode();
        root = newParent;
        pDataNode->setParent(newParent);
        newParent->setMostLeftChild(pDataNode);
    }

    


    // �θ� ��忡 �� Ű ����
    string firstKey = getFirstKey(rightSplit);
    rightSplit->setParent(pDataNode->getParent());
    pDataNode->getParent()->insertIndexMap(firstKey, rightSplit);

    // next, prev ������ ����
    rightSplit->setNext(pDataNode->getNext());
    if (pDataNode->getNext()) {
        pDataNode->getNext()->setPrev(rightSplit);
    }
    pDataNode->setNext(rightSplit);
    rightSplit->setPrev(pDataNode);
}

// �ε��� ��� ����
void BpTree::splitIndexNode(BpTreeNode* pIndexNode) {
    BpTreeNode* rightSplit = new BpTreeIndexNode();

    auto curIndexMap = pIndexNode->getIndexMap();
    auto splitIter = next(curIndexMap->begin(), curIndexMap->size() / 2);
    string middleKey = splitIter->first; // �߰� Ű�� �θ�� �ø�
    ++splitIter;

    // ������ �ε��� ���� ������ �̵�
    for (auto iter = splitIter; iter != curIndexMap->end();) {
        rightSplit->insertIndexMap(iter->first, iter->second);
        iter->second->setParent(rightSplit); // �θ� ��� ����
        iter = curIndexMap->erase(iter);
    }

    // �θ� ��尡 ������ ���ο� �ε��� ��� ����
    if (!pIndexNode->getParent()) {
        BpTreeNode* newParent = new BpTreeIndexNode();
        root = newParent;
        pIndexNode->setParent(newParent);
        newParent->setMostLeftChild(pIndexNode);
    }

    // �θ� ��忡 �߰� Ű ����
    rightSplit->setParent(pIndexNode->getParent());
    pIndexNode->getParent()->insertIndexMap(middleKey, rightSplit);
}

// ������ ��� �˻�
BpTreeNode* BpTree::searchDataNode(string n) {
    BpTreeNode* pCur = root;

    while (!pCur->isLeafNode()) {
        auto curIndexMap = pCur->getIndexMap();
        auto iter = curIndexMap->lower_bound(n);
        pCur = (iter == curIndexMap->end()) ? prev(curIndexMap->end())->second : iter->second;
    }
    return pCur;
}

// Ʈ�� ���
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

//������ �������ϱ�

bool BpTree::SearchRange(string start, string end) {
    BpTreeNode* cur = root;

    // ���� ���� ���� ���� �̵�
    while (cur && !cur->isLeafNode()) {
        cur = cur->getMostLeftChild();
    }

    bool found = false;
    // ���� ��� ��ȸ
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

    // ���� ���� ���� ���� �̵�
    while (!cur->isLeafNode()) {
        cur = cur->getMostLeftChild();
    }

    // ���� ��� ��ȸ
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