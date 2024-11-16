#include "AVLTree.h"

// ��ƿ��Ƽ �Լ� ����
namespace {
    AVLNode* insertRec(AVLNode* node, FlightData* pFlightData);
    AVLNode* rebalance(AVLNode* node);
    AVLNode* rotateLeft(AVLNode* x);
    AVLNode* rotateRight(AVLNode* y);
    int getBalanceFactor(AVLNode* node);
    int height(AVLNode* node);
    void inorderTraversal(AVLNode* node, vector<FlightData*>& v);
}

// ��� ����
bool AVLTree::Insert(FlightData* pFlightData) {
    root = insertRec(root, pFlightData);
    return root != nullptr;
}

// AVL Ʈ�� �˻�
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
    return nullptr; // ã�� ���� ���
}

// AVL Ʈ���� ���ͷ� ��ȯ
void AVLTree::GetVector(vector<FlightData*>& v) {
    inorderTraversal(root, v);
}

// --- ��ƿ��Ƽ �Լ� ���� ---

// ��� ���� ��� �Լ�
namespace {
    AVLNode* insertRec(AVLNode* node, FlightData* pFlightData) {
        if (!node) {
            // ���ο� ��� ����
            AVLNode* newNode = new AVLNode();
            newNode->setFlightData(pFlightData);
            return newNode;
        }

        // �װ���� �������� ���� ��ġ Ž��
        if (pFlightData->GetFlightNumber() < node->getFlightData()->GetFlightNumber()) {
            node->setLeft(insertRec(node->getLeft(), pFlightData));
        }
        else if (pFlightData->GetFlightNumber() > node->getFlightData()->GetFlightNumber()) {
            node->setRight(insertRec(node->getRight(), pFlightData));
        }
        else {
            // �ߺ��� �װ������ �������� ����
            return node;
        }

        // ���� �� ���� ������
        return rebalance(node);
    }
}

// ���� ��ȸ - ���Ϳ� ������ �߰�
namespace {
    void inorderTraversal(AVLNode* node, vector<FlightData*>& v) {
        if (!node) return;
        inorderTraversal(node->getLeft(), v);
        v.push_back(node->getFlightData());
        inorderTraversal(node->getRight(), v);
    }
}

// AVL Ʈ�� ���� ������
namespace {
    AVLNode* rebalance(AVLNode* node) {
        // ���� �μ� ���
        int balanceFactor = getBalanceFactor(node);

        // LL ȸ��
        if (balanceFactor > 1 && getBalanceFactor(node->getLeft()) >= 0)
            return rotateRight(node);

        // RR ȸ��
        if (balanceFactor < -1 && getBalanceFactor(node->getRight()) <= 0)
            return rotateLeft(node);

        // LR ȸ��
        if (balanceFactor > 1 && getBalanceFactor(node->getLeft()) < 0) {
            node->setLeft(rotateLeft(node->getLeft()));
            return rotateRight(node);
        }

        // RL ȸ��
        if (balanceFactor < -1 && getBalanceFactor(node->getRight()) > 0) {
            node->setRight(rotateRight(node->getRight()));
            return rotateLeft(node);
        }

        return node; // ������ ������ ���� ����
    }
}

// ��ȸ��
namespace {
    AVLNode* rotateLeft(AVLNode* x) {
        AVLNode* y = x->getRight();
        x->setRight(y->getLeft());
        y->setLeft(x);

        return y;
    }
}

// ��ȸ��
namespace {
    AVLNode* rotateRight(AVLNode* y) {
        AVLNode* x = y->getLeft();
        y->setLeft(x->getRight());
        x->setRight(y);

        return x;
    }
}

// ����� ���� �μ� ���
namespace {
    int getBalanceFactor(AVLNode* node) {
        if (!node) return 0;
        return height(node->getLeft()) - height(node->getRight());
    }
}

// ����� ���� ���
namespace {
    int height(AVLNode* node) {
        if (!node) return 0;
        return 1 + max(height(node->getLeft()), height(node->getRight()));
    }
}
