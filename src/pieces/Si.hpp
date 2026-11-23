#pragma once
#include "QuanCo.hpp"
class Si final : public QuanCo {
public:
    Piece::Type loai() const override { return Piece::Advisor; }
    std::vector<Move> nuocDi(const Board& banCo, int hang, int cot, Piece::Side phe) const override;
};
