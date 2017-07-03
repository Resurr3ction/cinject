#include <gtest/gtest.h>
#include "cinject/cinject.h"


using cinject::Container;
using cinject::InjectionContext;
using cinject::ComponentNotFoundException;
using cinject::CircularDependencyFound;


namespace SimpleResolve
{
    class IRunner
    {
    public:
        virtual ~IRunner() {}
    };

    class Cheetah : public IRunner
    {
    public:
        INJECT(Cheetah()) {}
    };

    TEST(CInjectTest, TestSimpleResolve)
    {
        Container c;
        c.bind<IRunner>().to<Cheetah>();

        std::shared_ptr<IRunner> runner = c.get<IRunner>();
        std::shared_ptr<IRunner> runner2 = c.get<IRunner>();

        EXPECT_EQ(1, runner.use_count());
        EXPECT_EQ(1, runner2.use_count());
        EXPECT_NE(nullptr, dynamic_cast<Cheetah*>(runner.get()));
        EXPECT_NE(nullptr, dynamic_cast<Cheetah*>(runner2.get()));
        EXPECT_NE(runner, runner2);
    }

    TEST(CInjectTest, TestSimpleResolve__Singleton)
    {
        Container c;
        c.bind<IRunner>().to<Cheetah>().InSingletonScope();

        std::shared_ptr<IRunner> runner = c.get<IRunner>();
        std::shared_ptr<IRunner> runner2 = c.get<IRunner>();

        EXPECT_EQ(3, runner.use_count());
        EXPECT_EQ(3, runner2.use_count());
        EXPECT_NE(nullptr, dynamic_cast<Cheetah*>(runner.get()));
        EXPECT_NE(nullptr, dynamic_cast<Cheetah*>(runner2.get()));
        EXPECT_EQ(runner, runner2);
    }

    TEST(CInjectTest, TestSimpleResolve_ToSelf)
    {
        Container c;
        c.bind<Cheetah>().to<Cheetah>();

        std::shared_ptr<Cheetah> runner = c.get<Cheetah>();
        std::shared_ptr<Cheetah> runner2 = c.get<Cheetah>();

        EXPECT_EQ(1, runner.use_count());
        EXPECT_EQ(1, runner2.use_count());
        EXPECT_NE(nullptr, runner);
        EXPECT_NE(nullptr, runner2);
        EXPECT_NE(runner, runner2);
    }

    TEST(CInjectTest, TestSimpleResolve_ToSelf__Singleton)
    {
        Container c;
        c.bind<Cheetah>().to<Cheetah>().InSingletonScope();

        std::shared_ptr<Cheetah> runner = c.get<Cheetah>();
        std::shared_ptr<Cheetah> runner2 = c.get<Cheetah>();

        EXPECT_EQ(3, runner.use_count());
        EXPECT_EQ(3, runner2.use_count());
        EXPECT_NE(nullptr, runner);
        EXPECT_NE(nullptr, runner2);
        EXPECT_EQ(runner, runner2);
    }

    TEST(CInjectTest, TestSimpleResolve_To_Function)
    {
        cinject::Container c;
        c.bind<IRunner>()
            .toFunction<Cheetah>([](InjectionContext*) { return std::make_shared<Cheetah>(); });

        std::shared_ptr<IRunner> runner = c.get<IRunner>();
        std::shared_ptr<IRunner> runner2 = c.get<IRunner>();

        EXPECT_EQ(1, runner.use_count());
        EXPECT_EQ(1, runner2.use_count());
        EXPECT_NE(nullptr, dynamic_cast<Cheetah*>(runner.get()));
        EXPECT_NE(nullptr, dynamic_cast<Cheetah*>(runner2.get()));
        EXPECT_NE(runner, runner2);
    }

    TEST(CInjectTest, TestSimpleResolve_To_Function__Singleton)
    {
        cinject::Container c;
        c.bind<IRunner>()
            .toFunction<Cheetah>([](InjectionContext*) { return std::make_shared<Cheetah>(); })
            .InSingletonScope();

        std::shared_ptr<IRunner> runner = c.get<IRunner>();
        std::shared_ptr<IRunner> runner2 = c.get<IRunner>();

        EXPECT_EQ(3, runner.use_count());
        EXPECT_EQ(3, runner2.use_count());
        EXPECT_NE(nullptr, dynamic_cast<Cheetah*>(runner.get()));
        EXPECT_NE(nullptr, dynamic_cast<Cheetah*>(runner2.get()));
        EXPECT_EQ(runner, runner2);
    }

    TEST(CInjectTest, TestSimpleResolve_To_Constant)
    {
        auto cheetah = std::make_shared<Cheetah>();

        cinject::Container c;
        c.bind<IRunner>().toConstant(cheetah);

        std::shared_ptr<IRunner> runner = c.get<IRunner>();
        std::shared_ptr<IRunner> runner2 = c.get<IRunner>();

        EXPECT_EQ(4, runner.use_count());
        EXPECT_EQ(4, runner2.use_count());
        EXPECT_NE(nullptr, dynamic_cast<Cheetah*>(runner.get()));
        EXPECT_NE(nullptr, dynamic_cast<Cheetah*>(runner2.get()));
        EXPECT_EQ(runner, runner2);
    }
}


namespace MultipleInterfaces
{
    class IRunner
    {
    public:
        virtual ~IRunner() {}
        virtual int RunSpeed() = 0;
    };

    class IWalker
    {
    public:
        virtual ~IWalker() {}
        virtual int WalkSpeed() = 0;
    };

    class IJumper
    {
    public:
        virtual ~IJumper() {}
        virtual int JumpHeight() = 0;
    };

    class Cheetah : public IJumper, public IRunner, public IWalker
    {
    public:
        INJECT(Cheetah()) {}
        int WalkSpeed() override { return 10; }
        int RunSpeed() override { return 120; }
        int JumpHeight() override { return 2; }
    };

    TEST(CInjectTest, TestMultipleInterfaces)
    {
        Container c;
        c.bind<IWalker, IJumper, IRunner>().to<Cheetah>();

        std::shared_ptr<IRunner> runner = c.get<IRunner>();
        std::shared_ptr<IWalker> walker = c.get<IWalker>();
        std::shared_ptr<IJumper> jumper = c.get<IJumper>();

        EXPECT_EQ(1, runner.use_count());
        EXPECT_EQ(1, walker.use_count());
        EXPECT_EQ(1, jumper.use_count());
        EXPECT_NE(nullptr, dynamic_cast<Cheetah*>(runner.get()));
        EXPECT_NE(nullptr, dynamic_cast<Cheetah*>(walker.get()));
        EXPECT_NE(nullptr, dynamic_cast<Cheetah*>(jumper.get()));
        EXPECT_EQ(120, runner->RunSpeed());
        EXPECT_EQ(10, walker->WalkSpeed());
        EXPECT_EQ(2, jumper->JumpHeight());
        EXPECT_NE(dynamic_cast<Cheetah*>(runner.get()), dynamic_cast<Cheetah*>(walker.get()));
        EXPECT_NE(dynamic_cast<Cheetah*>(jumper.get()), dynamic_cast<Cheetah*>(walker.get()));
        EXPECT_NE(dynamic_cast<Cheetah*>(jumper.get()), dynamic_cast<Cheetah*>(runner.get()));
    }

    TEST(CInjectTest, TestMultipleInterfaces__Singleton)
    {
        Container c;
        c.bind<IWalker, IJumper, IRunner>().to<Cheetah>().InSingletonScope();

        std::shared_ptr<IRunner> runner = c.get<IRunner>();
        std::shared_ptr<IWalker> walker = c.get<IWalker>();
        std::shared_ptr<IJumper> jumper = c.get<IJumper>();

        EXPECT_EQ(4, runner.use_count());
        EXPECT_EQ(4, walker.use_count());
        EXPECT_EQ(4, jumper.use_count());
        EXPECT_NE(nullptr, dynamic_cast<Cheetah*>(runner.get()));
        EXPECT_NE(nullptr, dynamic_cast<Cheetah*>(walker.get()));
        EXPECT_NE(nullptr, dynamic_cast<Cheetah*>(jumper.get()));
        EXPECT_EQ(120, runner->RunSpeed());
        EXPECT_EQ(10, walker->WalkSpeed());
        EXPECT_EQ(2, jumper->JumpHeight());
        EXPECT_EQ(dynamic_cast<Cheetah*>(runner.get()), dynamic_cast<Cheetah*>(walker.get()));
        EXPECT_EQ(dynamic_cast<Cheetah*>(jumper.get()), dynamic_cast<Cheetah*>(walker.get()));
    }
}

namespace NestedDependencies
{
    class INest
    {
    public:
        virtual ~INest() {}
    };

    class SpiderNest : public INest
    {
    public:
        INJECT(SpiderNest()) {}
    };

    class Spider
    {
    public:
        INJECT(Spider(std::shared_ptr<INest> nest)) :
            nest(nest)
        {}

        std::shared_ptr<INest> nest;
    };

    TEST(CInjectTest, TestNestedDependencies)
    {
        Container c;
        c.bind<Spider>().ToSelf();
        c.bind<INest>().to<SpiderNest>().InSingletonScope();

        std::shared_ptr<Spider> spider1 = c.get<Spider>();
        std::shared_ptr<Spider> spider2 = c.get<Spider>();
        std::shared_ptr<Spider> spider3 = c.get<Spider>();
        std::shared_ptr<INest> nest = c.get<INest>();

        EXPECT_NE(spider2.get(), spider1.get());
        EXPECT_NE(spider3.get(), spider1.get());
        EXPECT_NE(spider3.get(), spider2.get());

        EXPECT_EQ(1, spider1.use_count());
        EXPECT_EQ(1, spider2.use_count());
        EXPECT_EQ(1, spider3.use_count());
        EXPECT_EQ(5, nest.use_count());
        EXPECT_NE(nullptr, dynamic_cast<SpiderNest*>(nest.get()));
    }
}

namespace NestedDependenciesWithVector
{
    class ISnake
    {
    public:
        virtual ~ISnake() {}
    };

    class GrassSnake : public ISnake
    {
    public:
        INJECT(GrassSnake()) {}
    };

    class Python : public ISnake
    {
    public:
        INJECT(Python()) {}
    };

    class Mamba : public ISnake
    {
    public:
        INJECT(Mamba()) {}
    };

    class Viper : public ISnake
    {
    public:
        INJECT(Viper()) {}
    };



    class IMaterial
    {
    public:
        virtual ~IMaterial() {}
    };

    class Paper : public IMaterial
    {
    public:
        INJECT(Paper()) {}
    };


    class IEncyclopedy
    {
    public:
        virtual ~IEncyclopedy() {}
    };

    class SnakeEncyclopedy : public IEncyclopedy
    {
    public:
        INJECT(SnakeEncyclopedy(std::shared_ptr<IMaterial> material, std::vector<std::shared_ptr<ISnake>> snakes)) :
            material(material),
            snakes(snakes)
        {}

        std::shared_ptr<IMaterial> material;
        std::vector<std::shared_ptr<ISnake>> snakes;
    };


    TEST(CInjectTest, TestNestedDependencies)
    {
        Container c;
        c.bind<ISnake>().to<GrassSnake>();
        c.bind<ISnake>().to<Python>();
        c.bind<ISnake>().to<Mamba>();
        c.bind<ISnake>().to<Viper>();
        c.bind<IMaterial>().to<Paper>();
        c.bind<IEncyclopedy>().to<SnakeEncyclopedy>().InSingletonScope();

        std::shared_ptr<IEncyclopedy> encyclopedy = c.get<IEncyclopedy>();
        std::shared_ptr<IMaterial> material = c.get<IMaterial>();

        auto snakeEncyclopedy = dynamic_cast<SnakeEncyclopedy*>(encyclopedy.get());
        ASSERT_NE(nullptr, snakeEncyclopedy);

        ASSERT_EQ(4, snakeEncyclopedy->snakes.size());
        EXPECT_EQ(1, snakeEncyclopedy->snakes[0].use_count());
        EXPECT_EQ(1, snakeEncyclopedy->snakes[1].use_count());
        EXPECT_EQ(1, snakeEncyclopedy->snakes[2].use_count());
        EXPECT_EQ(1, snakeEncyclopedy->snakes[3].use_count());

        EXPECT_EQ(1, material.use_count());
        EXPECT_EQ(1, snakeEncyclopedy->material.use_count());
    }
}

namespace ComponentNotFound
{
    class IRunner
    {
    public:
        virtual ~IRunner() {}
    };

    class IWaterPool
    {
    public:
        virtual ~IWaterPool() {}
    };

    class Human : public IRunner
    {
    public:
        INJECT(Human(std::shared_ptr<IWaterPool> waterPool)) {}
    };

    TEST(CInjectTest, TestComponentNotFound)
    {
        Container c;

        ASSERT_THROW(c.get<IRunner>(), ComponentNotFoundException);
    }

    TEST(CInjectTest, TestNestedComponentNotFound)
    {
        Container c;

        c.bind<IRunner>().to<Human>();

        ASSERT_THROW(c.get<IRunner>(), ComponentNotFoundException);
    }
}

namespace ResolveCollection
{
    class ISnake
    {
    public:
        virtual ~ISnake() {}
    };

    class GrassSnake : public ISnake
    {
    public:
        INJECT(GrassSnake()) {}
    };

    class Python : public ISnake
    {
    public:
        INJECT(Python()) {}
    };

    class Mamba : public ISnake
    {
    public:
        INJECT(Mamba()) {}
    };

    class Viper : public ISnake
    {
    public:
        INJECT(Viper()) {}
    };

    TEST(CInjectTest, TestResolveCollection)
    {
        Container c;

        c.bind<ISnake>().to<GrassSnake>().InSingletonScope();
        c.bind<ISnake>().to<Python>().InSingletonScope();
        c.bind<ISnake>().to<Mamba>().InSingletonScope();
        c.bind<ISnake>().to<Viper>().InSingletonScope();

        std::shared_ptr<ISnake> snake = c.get<ISnake>();

        auto grassSnake = dynamic_cast<GrassSnake*>(snake.get());
        EXPECT_NE(nullptr, grassSnake);

        EXPECT_EQ(2, snake.use_count());

        std::vector<std::shared_ptr<ISnake>> allSnakes = c.get<std::vector<ISnake>>();

        EXPECT_EQ(3, snake.use_count());
        ASSERT_EQ(4, allSnakes.size());

        EXPECT_EQ(3, allSnakes[0].use_count()); // GrassSnake
        EXPECT_EQ(2, allSnakes[1].use_count());
        EXPECT_EQ(2, allSnakes[2].use_count());
        EXPECT_EQ(2, allSnakes[3].use_count());

        EXPECT_NE(nullptr, dynamic_cast<GrassSnake*>(allSnakes[0].get()));
        EXPECT_NE(nullptr, dynamic_cast<Python*>(allSnakes[1].get()));
        EXPECT_NE(nullptr, dynamic_cast<Mamba*>(allSnakes[2].get()));
        EXPECT_NE(nullptr, dynamic_cast<Viper*>(allSnakes[3].get()));
    }

    TEST(CInjectTest, TestResolveEmptyCollection)
    {
        Container c;

        std::vector<std::shared_ptr<ISnake>> allSnakes = c.get<std::vector<ISnake>>();

        ASSERT_EQ(0, allSnakes.size());
    }

    TEST(CInjectTest, TestResolveCollection_UsingSharedPtr)
    {
        Container c;

        c.bind<ISnake>().to<GrassSnake>().InSingletonScope();
        c.bind<ISnake>().to<Python>().InSingletonScope();
        c.bind<ISnake>().to<Mamba>().InSingletonScope();
        c.bind<ISnake>().to<Viper>().InSingletonScope();

        std::vector<std::shared_ptr<ISnake>> allSnakes = c.get<std::vector<std::shared_ptr<ISnake>>>();

        ASSERT_EQ(4, allSnakes.size());
    }
}

namespace BindManyToOne
{
    class IWalker
    {
    public:
        virtual ~IWalker() {}
        virtual int walk() = 0;
    };

    class IRunner
    {
    public:
        virtual ~IRunner() {}
        virtual int run() = 0;
    };

    class IJumper
    {
    public:
        virtual ~IJumper() {}
        virtual int jump() = 0;
    };

    class Human : public IWalker, public IRunner, public IJumper
    {
    public:
        INJECT(Human()) {}

        virtual int walk() { return 1; }
        virtual int run() { return 2; }
        virtual int jump() { return 3; }
    };

    TEST(CInjectTest, TestBindManyToOne)
    {
        Container c;

        // intentional order to not match the function implementation order
        c.bind<IRunner, IJumper, IWalker>().to<Human>().InSingletonScope();

        std::shared_ptr<IWalker> walker = c.get<IWalker>();
        std::shared_ptr<IRunner> runner = c.get<IRunner>();
        std::shared_ptr<IJumper> jumper = c.get<IJumper>();

        EXPECT_EQ(4, walker.use_count());
        EXPECT_EQ(4, runner.use_count());
        EXPECT_EQ(4, jumper.use_count());

        EXPECT_EQ(1, walker->walk());
        EXPECT_EQ(2, runner->run());
        EXPECT_EQ(3, jumper->jump());
    }
}

namespace CircularDependency
{
    class Middle;
    class End;

    class Start
    {
    public:
        INJECT(Start(std::shared_ptr<Middle> middle)) {}
    };

    class Middle
    {
    public:
        INJECT(Middle(std::shared_ptr<End> end)) {}
    };

    class End
    {
    public:
        INJECT(End(std::shared_ptr<Start> start)) {}
    };

    TEST(CInjectTest, TestCircularDependency)
    {
        Container c;

        // intentional order to not match the function implementation order
        c.bind<Start>().ToSelf();
        c.bind<Middle>().ToSelf();
        c.bind<End>().ToSelf();

        ASSERT_THROW(c.get<Start>(), CircularDependencyFound);
    }

    TEST(CInjectTest, TestCircularDependency_UsingToFunction)
    {
        Container c;

        // intentional order to not match the function implementation order
        c.bind<Start>().toFunction<Start>([](InjectionContext* c) { return std::make_shared<Start>(c->getContainer().get<Middle>(c)); });
        c.bind<Middle>().ToSelf();
        c.bind<End>().ToSelf();

        ASSERT_THROW(c.get<Start>(), CircularDependencyFound);
    }
}

namespace InjectionContextStack
{
    class Home
    {
    public:
        Home(const std::string& name) :
            name(name)
        {

        }

        std::string name;
    };

    class ISnake
    {
    public:
        virtual ~ISnake() {}
        virtual std::string getHomeName() = 0;
    };

    class GrassSnake : public ISnake
    {
    public:
        COMPONENT_NAME("GrassSnake");
        INJECT(GrassSnake(std::shared_ptr<Home> home)) : home(home) {}

        virtual std::string getHomeName() override { return home->name; }

        std::shared_ptr<Home> home;
    };

    class Python : public ISnake
    {
    public:
        COMPONENT_NAME("Python");
        INJECT(Python(std::shared_ptr<Home> home)) : home(home) {}

        virtual std::string getHomeName() override { return home->name; }

        std::shared_ptr<Home> home;
    };

    class Mamba : public ISnake
    {
    public:
        COMPONENT_NAME("Mamba");
        INJECT(Mamba(std::shared_ptr<Home> home)) : home(home) {}

        virtual std::string getHomeName() override { return home->name; }

        std::shared_ptr<Home> home;
    };

    class Viper : public ISnake
    {
    public:
        COMPONENT_NAME("Viper");
        INJECT(Viper(std::shared_ptr<Home> home)) : home(home) {}

        virtual std::string getHomeName() override { return home->name; }

        std::shared_ptr<Home> home;
    };




    TEST(CInjectTest, TestInjectionContextStack_Name)
    {
        Container c;

        // intentional order to not match the function implementation order
        c.bind<Home>().toFunction<Home>([](InjectionContext* c) { return std::make_shared<Home>(c->getRequester().name() + "'s home"); });
        c.bind<ISnake>().to<GrassSnake>();
        c.bind<ISnake>().to<Python>();
        c.bind<ISnake>().to<Mamba>();
        c.bind<ISnake>().to<Viper>();

        std::vector<std::shared_ptr<ISnake>> snakes = c.get<std::vector<ISnake>>();

        ASSERT_EQ(4, snakes.size());
        ASSERT_NE(nullptr, dynamic_cast<GrassSnake*>(snakes[0].get()));
        ASSERT_NE(nullptr, dynamic_cast<Python*>(snakes[1].get()));
        ASSERT_NE(nullptr, dynamic_cast<Mamba*>(snakes[2].get()));
        ASSERT_NE(nullptr, dynamic_cast<Viper*>(snakes[3].get()));

        ASSERT_EQ(std::string("GrassSnake's home"), snakes[0]->getHomeName());
        ASSERT_EQ(std::string("Python's home"), snakes[1]->getHomeName());
        ASSERT_EQ(std::string("Mamba's home"), snakes[2]->getHomeName());
        ASSERT_EQ(std::string("Viper's home"), snakes[3]->getHomeName());
    }
}

namespace ContainerHierarchy
{
    class City
    {
    public:
        INJECT(City()) {}
    };

    class Building
    {
    public:
        INJECT(Building()) {}
    };

    TEST(CInjectTest, TestInjectionContextStack_Name)
    {
        Container c;

        c.bind<City>().ToSelf().InSingletonScope();

        Container child(&c);
        child.bind<Building>().ToSelf().InSingletonScope();


        std::shared_ptr<Building> building = child.get<Building>();
        std::shared_ptr<City> city = child.get<City>();

        std::shared_ptr<City> city2 = c.get<City>();

        EXPECT_EQ(city, city2);
        EXPECT_THROW(c.get<Building>(), ComponentNotFoundException);
    }
}

namespace ContainerHierarchyWithCollection
{
    class IAnimal
    {
    public:
        virtual ~IAnimal() {}
    };

    class Snake : public IAnimal
    {
    public:
        INJECT(Snake()) {}
    };

    class Cheetah : public IAnimal
    {
    public:
        INJECT(Cheetah()) {}
    };

    class Bird : public IAnimal
    {
    public:
        INJECT(Bird()) {}
    };

    class Fish : public IAnimal
    {
    public:
        INJECT(Fish()) {}
    };

    TEST(CInjectTest, TestInjectionContextStack_Name)
    {
        Container c;

        c.bind<IAnimal>().to<Fish>().InSingletonScope();
        c.bind<IAnimal>().to<Bird>().InSingletonScope();

        Container child(&c);
        child.bind<IAnimal>().to<Snake>().InSingletonScope();
        child.bind<IAnimal>().to<Cheetah>().InSingletonScope();


        std::vector<std::shared_ptr<IAnimal>> animalsFromRoot = c.get<std::vector<IAnimal>>();
        std::vector<std::shared_ptr<IAnimal>> animalsFromChild = child.get<std::vector<IAnimal>>();

        ASSERT_EQ(2, animalsFromRoot.size());
        ASSERT_EQ(4, animalsFromChild.size());


    }
}