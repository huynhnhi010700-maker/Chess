#pragma once
#include "../core/Piece.hpp"
#include "../core/Move.hpp"
#include <vector>
class Board; // forward declaration: Board va QuanCo phu thuoc nhau
class QuanCo {
public:
    virtual ~QuanCo() = default;
    virtual Piece::Type loai() const = 0;
    virtual std::vector<Move> nuocDi(const Board& banCo, int hang, int cot, Piece::Side phe) const = 0;
};
