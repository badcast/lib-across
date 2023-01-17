﻿#include <across.hpp>

namespace across
{

#define MEMORY_DATA (((std::uint8_t*)neurons) + ((range.x * heightSpace + range.y) * AcrossDataSizeMultiplier) + segmentOffset)

    template<typename Vec2Int = APoint>
    class AlgorithmUtils
    {
    public:
        ///Определяет дистанцию точки от A до точки B
        ///Используется формула Пифагора "(a^2) + (b^2) = c^2"
        ///\par lhs Первоначальная точка
        ///\par rhs Конечная точка
        ///\return Сумма
        static inline int DistancePhf(const Vec2Int& lhs, const Vec2Int& rhs);

        ///Определяет дистанцию точки от A до точки B
        ///\par lhs Первоначальная точка
        ///\par rhs Конечная точка
        ///\return Сумма
        static inline int DistanceManht(const Vec2Int& lhs, const Vec2Int& rhs);

        /// Определяет, минимальную стоимость
        static auto GetMinCostPath(AcrossMap& map, std::list<Vec2Int>* paths) -> decltype(std::begin(*paths));

        /// Определяет, функцию пойска по направлениям. Таких как: left, up, right,
        static void AvailPoints(AcrossMap& map, NavMethodRule method, Vec2Int arrange, Vec2Int target, std::list<Vec2Int>* pathTo, std::size_t maxCount = -1, int filterFlag = -1);
    };

    AcrossMap::AcrossMap(int lwidth, int lheight)
    {
        if (!lwidth || !lheight)
            throw std::runtime_error("Width or Height is zero!");

        this->widthSpace = lwidth;
        this->heightSpace = lheight;
        this->worldScale = Vec2::one;
        std::div_t lockedDiv = div(Math::max(lheight = (lwidth * lheight), 8), 8); // add locked bits
        segmentOffset = lwidth = lockedDiv.quot + lockedDiv.rem;
        this->neurons = std::malloc(lheight * AcrossDataSizeMultiplier + lwidth);

        clear(true);
    }

    AcrossMap::~AcrossMap() { std::free(this->neurons); }

    void AcrossMap::randomGenerate(int flagFilter)
    {
        std::uint32_t lhs, rhs = segmentOffset;
        Neuron* p;
        clear(true);
        do {
            lhs = static_cast<std::uint32_t>(rand() & flagFilter);
            memcpy(reinterpret_cast<std::int8_t*>(neurons) + segmentOffset - rhs, &lhs, Math::min(rhs, (std::uint32_t)sizeof(int)));
            rhs -= Math::min(rhs, static_cast<std::uint32_t>(sizeof(int)));
        } while (rhs > 0);
    }

    void AcrossMap::stress()
    {
        NavResult result;
        Vec2Int first, next = { static_cast<int>(widthSpace - 1), static_cast<int>(heightSpace - 1) };
        // TODO: next a strees
        find(result, NavMethodRule::NavigationIntelegency, first, next, NavAlgorithm::AStar);
    }

    int AcrossMap::getWidth() { return widthSpace; }

    int AcrossMap::getHeight() { return heightSpace; }

    void AcrossMap::clear(bool clearLocks)
    {
        std::uint32_t length = widthSpace * heightSpace * AcrossDataSizeMultiplier;
        std::uint32_t leftOffset;
        if (clearLocks) {
            leftOffset = 0;
            length += this->segmentOffset;
        } else {
            leftOffset = this->segmentOffset;
        }
        memset(reinterpret_cast<std::uint8_t*>(neurons) + leftOffset, 0, length);
    }

    void AcrossMap::fill(bool fillLocks)
    {
        std::uint32_t length = widthSpace * heightSpace * AcrossDataSizeMultiplier;
        std::uint32_t leftoffset;
        if (!fillLocks) {
            leftoffset = this->segmentOffset;
        } else {
            leftoffset = 0;
            length += this->segmentOffset;
        }
        memset(reinterpret_cast<std::uint8_t*>(neurons) + leftoffset, 0xff, length);
    }

    Neuron* AcrossMap::GetNeuron(int x, int y) { return GetNeuron(Vec2Int(x, y)); }

    Neuron* AcrossMap::GetNeuron(const Vec2Int& range)
    {
        Neuron* result = nullptr;
        if (neuronContains(range))
            result = reinterpret_cast<Neuron*>(MEMORY_DATA);
        return result;
    }

    const Vec2Int AcrossMap::WorldPointToPoint(const Vec2& worldPoint)
    {
        Vec2Int p;
        p.x = Math::ceil(widthSpace / 2 + (worldPoint.x + worldOffset.x) / worldScale.x);
        p.y = Math::ceil(heightSpace / 2 - (worldPoint.y + worldOffset.y) / worldScale.y);
        return p;
    }

    std::uint32_t AcrossMap::getCachedSize()
    {
        std::uint32_t cal = 0;

        Neuron* n = static_cast<Neuron*>(neurons + segmentOffset);
        Neuron* nM = n + widthSpace * heightSpace;
        while (n < nM) {
            cal += n->cost + n->h;
            ++n;
        }
        return cal;
    }

    Neuron* AcrossMap::GetNeuron(const Vec2& worldPoint)
    {
        Vec2Int p;
        p.x = Math::ceil(widthSpace / 2 + worldPoint.x / worldScale.x);
        p.y = Math::ceil(heightSpace / 2 - worldPoint.y / worldScale.y);
        return GetNeuron(p);
    }
    Neuron* AcrossMap::GetNeuron(const Vec2& worldPoint, Vec2Int& outPoint)
    {
        outPoint.x = Math::ceil(widthSpace / 2 + (worldPoint.x + worldOffset.x) / worldScale.x);
        outPoint.y = Math::ceil(heightSpace / 2 - (worldPoint.y + worldOffset.y) / worldScale.y);
        return GetNeuron(outPoint);
    }

    const Vec2 AcrossMap::PointToWorldPosition(const Vec2Int& range) { return PointToWorldPosition(range.x, range.y); }
    const Vec2 AcrossMap::PointToWorldPosition(Neuron* neuron) { return PointToWorldPosition(neuronGetPoint(neuron)); }

    const Vec2 AcrossMap::PointToWorldPosition(const int& x, const int& y)
    {
        Vec2 vec2(widthSpace / 2.f, heightSpace / 2.f);
        vec2.x = (x - vec2.x) * worldScale.x;
        vec2.y = -(y - vec2.y) * worldScale.y;
        return vec2 + worldOffset;
    }

    bool RoninEngine::AIPathFinder::Across::neuronLocked(const Vec2Int& range)
    {
        if (!neuronContains(range))
            throw std::out_of_range("range");
        auto divide = std::div(range.x * heightSpace + range.y, 8);
        auto pointer = reinterpret_cast<std::uint8_t*>(neurons) + divide.quot;
        return (*pointer) & (1 << divide.rem);
    }

    bool RoninEngine::AIPathFinder::Across::neuronContains(const Vec2Int& range) { return range.x > ~0 && range.y > ~0 && range.x <= widthSpace && range.y <= heightSpace; }

    std::uint8_t& RoninEngine::AIPathFinder::Across::neuronGetFlag(const Vec2Int& range)
    {
        Neuron* n = GetNeuron(range);
        if (!n)
            throw std::out_of_range("range");
        return n->flags;
    }

    std::uint32_t& RoninEngine::AIPathFinder::Across::neuronGetCost(const Vec2Int& range)
    {
        Neuron* n = GetNeuron(range);
        if (!n)
            throw std::out_of_range("range");
        return n->cost;
    }

    std::uint32_t& RoninEngine::AIPathFinder::Across::neuronHeuristic(const Vec2Int& range)
    {
        Neuron* n = GetNeuron(range);
        if (!n)
            throw std::out_of_range("range");
        return n->h;
    }

    const int RoninEngine::AIPathFinder::Across::neuronGetWeight(const Vec2Int& range)
    {
        if (!neuronContains(range))
            throw std::out_of_range("range");
        return range.x * range.y + range.y * range.y;
    }

    const std::uint32_t RoninEngine::AIPathFinder::Across::neuronGetTotal(const Vec2Int& range)
    {
        Neuron* n = GetNeuron(range);
        if (!n)
            throw std::out_of_range("range");
        return n->cost + n->h;
    }

    const bool RoninEngine::AIPathFinder::Across::neuronEmpty(const Vec2Int& range) { return !neuronGetTotal(range); }

    bool AcrossMap::neuronLocked(const Neuron* neuron) { return neuronLocked(neuronGetPoint(neuron)); }
    std::uint8_t& AcrossMap::neuronGetFlag(const Neuron* neuron) { return neuronGetFlag(neuronGetPoint(neuron)); }
    std::uint32_t& AcrossMap::neuronGetCost(const Neuron* neuron) { return neuronGetCost(neuronGetPoint(neuron)); }
    std::uint32_t& AcrossMap::neuronHeuristic(const Neuron* neuron) { return neuronHeuristic(neuronGetPoint(neuron)); }
    const int AcrossMap::neuronGetWeight(const Neuron* neuron) { return neuronGetWeight(neuronGetPoint(neuron)); }
    const std::uint32_t AcrossMap::neuronGetTotal(const Neuron* neuron) { return neuronEmpty(neuronGetPoint(neuron)); }
    const bool AcrossMap::neuronEmpty(const Neuron* neuron) { return neuronEmpty(neuronGetPoint(neuron)); }
    void AcrossMap::neuronLock(const Neuron* neuron, const bool state) { neuronLock(neuronGetPoint(neuron), state); }

    const Vec2Int AcrossMap::neuronGetPoint(const Neuron* neuron)
    {
        if (neuron == nullptr)
            throw std::runtime_error("argument is null");
        auto divide = std::div((reinterpret_cast<std::size_t>(neuron) - reinterpret_cast<std::size_t>(neurons) - segmentOffset) / AcrossDataSizeMultiplier, heightSpace);
        return { divide.quot, divide.rem };
    }

    void RoninEngine::AIPathFinder::Across::neuronLock(const Vec2Int& range, const bool state)
    {
        auto divide = std::div(range.x * heightSpace + range.y, 8);
        auto&& pointer = (reinterpret_cast<std::uint8_t*>(neurons) + segmentOffset) + divide.quot;
        divide.quot = (1 << divide.rem);
        // TODO: Optimized to ~divide.quout (xor)
        (*pointer) ^= (*pointer) & (divide.quot);
        (*pointer) |= divide.quot * (state == true);
    }

    void AcrossMap::load(const AcrossData& AcrossData)
    {
        if (!AcrossData.widthSpace || !AcrossData.heightSpace)
            throw std::runtime_error("Argument param, width or height is empty");

        if (AcrossData.neurons == nullptr)
            throw std::runtime_error("Data is null");

        if (this->neurons == AcrossData.neurons)
            throw std::runtime_error("neurons == Across data, unflow effect");

        if (this->neurons != nullptr)
            GC::gc_free(this->neurons);

        this->widthSpace = AcrossData.widthSpace;
        this->heightSpace = AcrossData.heightSpace;
        this->neurons = GC::gc_malloc(widthSpace * heightSpace * sizeof(Neuron));

        memcpy(this->neurons, AcrossData.neurons, widthSpace * heightSpace * sizeof(Neuron));
    }

    void AcrossMap::save(AcrossData* AcrossData)
    {
        if (AcrossData == nullptr)
            return;

        AcrossData->widthSpace = this->widthSpace;
        AcrossData->heightSpace = this->heightSpace;
        AcrossData->neurons = this->neurons;
    }

    void AcrossMap::find(NavResult& navResult, NavMethodRule method, Neuron* firstNeuron, Neuron* lastNeuron, NavAlgorithm algorithm) { find(navResult, method, neuronGetPoint(firstNeuron), neuronGetPoint(lastNeuron), algorithm); }

    void AcrossMap::find(NavResult& navResult, NavMethodRule method, Vec2Int first, Vec2Int last, NavAlgorithm algorithm)
    {
        Neuron *current, *firstNeuron, *lastNeuron, *select;
        navResult.map = this;
        navResult.firstNeuron = first;
        navResult.lastNeuron = last;
        navResult.algorithm = algorithm;

        if ((firstNeuron = GetNeuron(first)) == nullptr || (lastNeuron = GetNeuron(last)) == nullptr) {
            navResult.status = NavStatus::Undefined;
            return;
        }
        if (neuronLocked(first) || neuronLocked(last)) {
            navResult.status = NavStatus::Locked;
            return;
        }

        enum : std::uint8_t { FLAG_OPEN_LIST = 1, FLAG_CLOSED_LIST = 2, FLAG_TILED_LIST = 4 };

        std::list<Vec2Int> finded;
        decltype(navResult.RelativePaths)::iterator iter, p1, p2;
        navResult.RelativePaths.clear();
        navResult.RelativePaths.emplace_back(first);
        firstNeuron->h = AlgorithmUtils::DistanceManht(first, last);

        //Перестройка
        navResult.status = NavStatus::Opened;
        while (!navResult.RelativePaths.empty()) {
            iter = navResult.RelativePaths.begin(); // get the best Neuron
            current = GetNeuron(iter.operator*());
            if (current == lastNeuron) {
                break;
            }

            current->flags = FLAG_CLOSED_LIST; // change to closing list
            navResult.RelativePaths.erase(iter);

            // Avail

            AlgorithmUtils::AvailPoints(*this, method, this->neuronGetPoint(current), last, &finded);
            for (iter = std::begin(finded); iter != std::end(finded); ++iter) {
                select = GetNeuron(*iter);
                if (!(select->flags)) // free path
                {
                    select->flags = FLAG_OPEN_LIST;
                    select->cost = current->cost + 1;
                    select->h = AlgorithmUtils::DistanceManht((*iter), last);

                    navResult.RelativePaths.emplace_back((*iter));
                    p1 = std::begin(navResult.RelativePaths);
                    p2 = std::end(navResult.RelativePaths);

                    for (; p1 != p2;) {
                        if (neuronGetTotal(*p1) > neuronGetTotal(*iter)) {
                            navResult.RelativePaths.emplace(p1, (*iter));
                            break;
                        }
                        ++p1;
                    }
                    navResult.RelativePaths.emplace_back((*iter));
                }
            }

            finded.clear(); // clear data
        }

        Neuron* lastSelect = nullptr;
        current = lastNeuron;
        navResult.RelativePaths.clear();
        navResult.RelativePaths.emplace_back(last);
        while (current != firstNeuron) {
            if (current == lastSelect) {
                navResult.status = NavStatus::Closed;
                break;
            }
            lastSelect = current;
            AlgorithmUtils::AvailPoints(*this, method, neuronGetPoint(current), first, &finded, -1, FLAG_OPEN_LIST | FLAG_CLOSED_LIST);
            for (iter = std::begin(finded); iter != std::end(finded); ++iter) {
                select = GetNeuron(*iter);
                if ((select->cost && select->cost < current->cost) || select == firstNeuron) {
                    current = select;
                    navResult.RelativePaths.emplace_front(*iter);
                    current->flags = FLAG_TILED_LIST;
                }
            }

            finded.clear();
        }
    }

    int AlgorithmUtils::DistancePhf(const Vec2Int& a, const Vec2Int& b)
    {
        return pow(a.x - b.x, 2) + pow(a.y - b.y, 2); // a->x * a->y + b->x * b->y;
    }

    int AlgorithmUtils::DistanceManht(const Vec2Int& a, const Vec2Int& b) { return abs(b.x - a.x) + abs(b.y - a.y); }

    auto AlgorithmUtils::GetMinCostPath(AcrossMap& map, std::list<Vec2Int>* paths) -> decltype(std::begin(*paths))
    {
        int min = std::numeric_limits<int>::max();
        auto result = begin(*paths);
        for (auto i = result; i != std::end(*paths); ++i) {
            int g = map.neuronGetTotal(*i);
            if (g <= min) {
                min = g;
                result = i;
            }
        }
        return result;
    }

    int GetMatrixMethod(NavMethodRule method, std::int8_t** matrixH, std::int8_t** matrixV)
    {
        static std::int8_t PLUS_H_POINT[] { -1, 1, 0, 0 };
        static std::int8_t PLUS_V_POINT[] { 0, 0, -1, 1 };

        static std::int8_t M_SQUARE_H_POINT[] { -1, 0, 1, -1, 1, -1, 0, 1 };
        static std::int8_t M_SQUARE_V_POINT[] { -1, -1, -1, 0, 0, 1, 1, 1 };

        static std::int8_t M_CROSS_H_POINT[] { -1, 1, -1, 1 };
        static std::int8_t M_CROSS_V_POINT[] { -1, -1, 1, 1 };

        switch (method) {
        case RoninEngine::AIPathFinder::PlusMethod:
            *matrixH = PLUS_H_POINT;
            *matrixV = PLUS_V_POINT;
            return sizeof(PLUS_H_POINT);
            break;
        case RoninEngine::AIPathFinder::SquareMethod:
            *matrixH = M_SQUARE_H_POINT;
            *matrixV = M_SQUARE_V_POINT;
            return sizeof(M_SQUARE_H_POINT);
            break;
        case RoninEngine::AIPathFinder::CrossMethod:
            *matrixH = M_CROSS_H_POINT;
            *matrixV = M_CROSS_V_POINT;
            return sizeof(M_CROSS_H_POINT);
            break;
        }
        return 0;
    }

    void AlgorithmUtils::AvailPoints(AcrossMap& map, NavMethodRule method, Vec2Int arrange, Vec2Int target, std::list<Vec2Int>* pathTo, std::size_t maxCount, int filterFlag)
    {
        Neuron* it = nullptr;
        Vec2Int point;
        int i = 0, c;
        std::int8_t* matrixH;
        std::int8_t* matrixV;
        switch (method) {
        case RoninEngine::AIPathFinder::NavMethodRule::NavigationIntelegency: {
            // TODO: Написать интелектуальный пойск путей для достижения лучших
            // результатов.
            // TODO: Приводить вектор направление для лучшего достижения.
            // TODO: Выводить оптимальный результат, чтобы было меньше итерации
            // TODO: Вывести итог и анализ скорости.
            c = GetMatrixMethod(NavMethodRule::SquareMethod, &matrixH, &matrixV);
            do {
                point.x = arrange.x + matrixH[i];
                point.y = arrange.y + matrixV[i];
                it = map.GetNeuron(point);
                if (it && !map.neuronLocked(point) && (filterFlag == ~0 || it->flags & filterFlag)) {
                    if (point.x == target.x || point.y == target.y) {
                        if (point == target) {
                            i = c;
                            pathTo->clear();
                        }
                        pathTo->emplace_front(point);
                    } else
                        pathTo->emplace_back(point);
                }
                // next step
            } while (maxCount != pathTo->size() && i++ != c);
        } break;
        default:

            c = GetMatrixMethod(method, &matrixH, &matrixV);
            for (; i != c; ++i) {
                point.x = arrange.x + matrixH[i];
                point.y = arrange.y + matrixV[i];
                it = map.GetNeuron(point);
                if (it && !map.neuronLocked(point) && (filterFlag == ~0 || it->flags & filterFlag)) {
                    pathTo->emplace_back(point);
                    if (maxCount == pathTo->size() || point == target)
                        break;
                }
            }

            break;
        }
    }

} // namespace RoninEngine::AIPathFinder
#undef MEMORY_DATA
