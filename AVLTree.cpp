#include "AVLTree.h"

// 유틸리티 함수 선언
namespace {
    AVLNode* insertRec(AVLNode* node, FlightData* pFlightData);
    AVLNode* rebalance(AVLNode* node);
    AVLNode* rotateLeft(AVLNode* x);
    AVLNode* rotateRight(AVLNode* y);
    int getBalanceFactor(AVLNode* node);
    int height(AVLNode* node);
    void inorderTraversal(AVLNode* node, vector<FlightData*>& v);
}

// 노드 삽입
bool AVLTree::Insert(FlightData* pFlightData) {
    root = insertRec(root, pFlightData);
    return root != nullptr;
}

// AVL 트리 검색
FlightData* AVLTree::Search(string name) {
    AVLNode* current = root;

    while (current) {
        if (name == current->getFlightData()->GetFlightNumber()) {
            return current->getFlightData();
        }
        else if (name < current->getFlightData()->GetFlightNumber()) {
            current = current->getLeft();
        }
        else {
            current = current->getRight();
        }
    }
    return nullptr; // 찾지 못한 경우
}

// AVL 트리를 벡터로 변환
void AVLTree::GetVector(vector<FlightData*>& v) {
    inorderTraversal(root, v);
}

// --- 유틸리티 함수 정의 ---

// 노드 삽입 재귀 함수
namespace {
    AVLNode* insertRec(AVLNode* node, FlightData* pFlightData) {
        if (!node) {
            // 새로운 노드 생성
            AVLNode* newNode = new AVLNode();
            newNode->setFlightData(pFlightData);
            return newNode;
        }

        // 항공편명 기준으로 삽입 위치 탐색
        if (pFlightData->GetFlightNumber() < node->getFlightData()->GetFlightNumber()) {
            node->setLeft(insertRec(node->getLeft(), pFlightData));
        }
        else if (pFlightData->GetFlightNumber() > node->getFlightData()->GetFlightNumber()) {
            node->setRight(insertRec(node->getRight(), pFlightData));
        }
        else {
            // 중복된 항공편명은 삽입하지 않음
            return node;
        }

        // 삽입 후 균형 재조정
        return rebalance(node);
    }
}

// 중위 순회 - 벡터에 데이터 추가
namespace {
    void inorderTraversal(AVLNode* node, vector<FlightData*>& v) {
        if (!node) return;
        inorderTraversal(node->getLeft(), v);
        v.push_back(node->getFlightData());
        inorderTraversal(node->getRight(), v);
    }
}

// AVL 트리 균형 재조정
namespace {
    AVLNode* rebalance(AVLNode* node) {
        // 균형 인수 계산
        int balanceFactor = getBalanceFactor(node);

        // LL 회전
        if (balanceFactor > 1 && getBalanceFactor(node->getLeft()) >= 0)
            return rotateRight(node);

        // RR 회전
        if (balanceFactor < -1 && getBalanceFactor(node->getRight()) <= 0)
            return rotateLeft(node);

        // LR 회전
        if (balanceFactor > 1 && getBalanceFactor(node->getLeft()) < 0) {
            node->setLeft(rotateLeft(node->getLeft()));
            return rotateRight(node);
        }

        // RL 회전
        if (balanceFactor < -1 && getBalanceFactor(node->getRight()) > 0) {
            node->setRight(rotateRight(node->getRight()));
            return rotateLeft(node);
        }

        return node; // 균형이 맞으면 변경 없음
    }
}

// 좌회전
namespace {
    AVLNode* rotateLeft(AVLNode* x) {
        AVLNode* y = x->getRight();
        x->setRight(y->getLeft());
        y->setLeft(x);

        return y;
    }
}

// 우회전
namespace {
    AVLNode* rotateRight(AVLNode* y) {
        AVLNode* x = y->getLeft();
        y->setLeft(x->getRight());
        x->setRight(y);

        return x;
    }
}

// 노드의 균형 인수 계산
namespace {
    int getBalanceFactor(AVLNode* node) {
        if (!node) return 0;
        return height(node->getLeft()) - height(node->getRight());
    }
}

// 노드의 높이 계산
namespace {
    int height(AVLNode* node) {
        if (!node) return 0;
        return 1 + max(height(node->getLeft()), height(node->getRight()));
    }
}
