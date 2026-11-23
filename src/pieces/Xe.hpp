#pragma once
#include "QuanCo.hpp"
class Xe final : public QuanCo {
public:
    Piece::Type loai() const override { return Piece::Rook; }
    std::vector<Move> nuocDi(const Board& banCo, int hang, int cot, Piece::Side phe) const override;
};
