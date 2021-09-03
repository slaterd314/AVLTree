#ifdef WIN32
#define WIN32_LEAN_AND_MEAN 1
#define NOMINMAX 1
#include <windows.h>
#endif
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <limits>
#include <cerrno>
#include <fstream>
#include "AVLTreeT.h"
#include "PrintTree.h"
#include "VMVirtAddrManager.h"
#include "BinaryTreeNodeT.h"

using namespace std;
using node2 = BinaryTreeNodeT<uint64_t>;

string generateHtml(const string &dotsArrayStr);
extern const string htmlTemplate1;
extern const string htmlTemplate2;

static void runTest();
static void test2();

auto foo = AVLTreeT<node2>{};
static auto dottyOutput = false;
int main(int argc, char **argv)
{
#ifdef WIN32
    SetConsoleOutputCP(65001);
#endif        
    if(argc > 1)
    {
        // auto i = 1;
        if( *(argv[1]) == '-' && *(argv[1]+1) == 'd' )
        {
            dottyOutput = true;
        }
#ifdef DEBUG
        filebuf buffer{};
        auto *oldBuf = cout.rdbuf();
        auto *newBuf = buffer.open("AnimatedTree.html", ios_base::out | ios_base::trunc);
        if (oldBuf && newBuf)
        {
            cout.rdbuf(newBuf);
            cout << htmlTemplate1;
        }
#endif
        AVLTreeT<node2::NodePtr, node2::traits> tree{};
        for( auto i=1; i<argc; ++i )
        {
            char *strend = nullptr;
            const char *str = argv[i];
            errno = 0;
            
            
                uint64_t data = std::strtoull(str, &strend, 10);
                if (errno == 0)
                {
                    // auto node = make_node<uint64_t>(data);
                    auto node = node2::create(data);
                    if (node)
                    {
                        auto success = tree.insertNode(std::move(node));
                        if (!success)
                        {
                            std::cerr << "Error: insertNode FAILED: " << str << "\n";
                        }
                    }
                }
                else
                {
                    std::cerr << "Error: bad integer: " << str << std::strerror(errno) << "\n";
                }
        }

        if( dottyOutput )
        {
            dottyTree(tree);
        }
        else
        {
#if 0
            cout << "================ Iterate the tree ==================================\n";
            for (const auto &n : tree)
            {
                cout << n->data() << ", ";
            }
            cout << "\n====================================================================\n";
            printTree<node2::NodePtr &, node2::traits>(tree.rootNode(), std::shared_ptr<Trunk>{}, false);
            cout << "\n=====================Delete tree=====================================\n";
#endif
            while(tree.rootNode())
            {
                tree.removeNode(tree.rootNode());
            }
#ifdef DEBUG
            if (oldBuf && newBuf)
            {
                cout << htmlTemplate2 << endl;
                cout.flush();
                cout.rdbuf(oldBuf);
            }
#endif

        }        
    }
    else
    {
        test2();
       // runTest();
    }
    return 0;
}

static void lowerBound(AVLTreeT<node2::NodePtr, node2::traits> &t, uint64_t v)
{
    // node2::NodePtr node = make_node<uint64_t>(v);
    node2::NodePtr node = node2::create(v);
    // node.data = v;
    const auto &n = t.lowerBound(node);
    if(n)
    {
        cout << "Lower Bound: " << v << " == " << n->data() << '\n';
    }
    else
    {
        cout << "Lower Bound: " << v << " Failed\n";
    }
}

static void runTest()
{
    AVLTreeT<node2::NodePtr, node2::traits> tree{};
    
    for( auto i=0ull; i<=50ull; ++i)
    {
        // auto node = make_node<uint64_t>(i * 2);
        auto node = node2::create(i * 2);
        tree.insertNode(std::move(node));
    }


    lowerBound(tree, 45);
    lowerBound(tree, 44);
    lowerBound(tree, 46);

    lowerBound(tree, 3);

    lowerBound(tree, 99);
    
}


static void test2()
{
    VMVirtAddrManager mgr{};
    VirtBlock blocks[] = {
        VirtBlock{ reinterpret_cast<void *>(0x0000000000001000), 1ull },
        VirtBlock{ reinterpret_cast<void *>(0x0000000000002000), 64ull },
        VirtBlock{ reinterpret_cast<void *>(0x000000000C002000), 0x1000ull },
        VirtBlock{ reinterpret_cast<void *>(0x000000000C00A000), 0x3ull },
        VirtBlock{ reinterpret_cast<void *>(0x000000000C01A000), 0x128ull }
    };
    const auto count = sizeof(blocks)/sizeof(blocks[0]);

    for( auto i=0; i<count; ++i)
    {
        mgr.addBlock(blocks[i]);    
    }


    std::cout << "digraph {\n";
    std::cout << std::resetiosflags( std::cout.boolalpha );
    dottyTree<const VMNode *, VMNodeTraitsEW>(mgr.addressTree.rootNode());
    std::cout << std::setiosflags( std::cout.boolalpha );
    dottyTree<const VMNode *, VMNodeTraitsNS>(mgr.blockSizeTree.rootNode());
    std::cout << "}" << std::endl;

    return;

}