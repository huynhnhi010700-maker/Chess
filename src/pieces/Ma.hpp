#pragma once
#include "QuanCo.hpp"
class Ma final : public QuanCo {
public:
    Piece::Type loai() const override { return Piece::Horse; }
    std::vector<Move> nuocDi(const Board& banCo, int hang, int cot, Piece::Side phe) const override;
};
