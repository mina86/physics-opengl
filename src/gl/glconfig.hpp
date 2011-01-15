/*
 * src/gl/glconfig.hpp
 * Copyright 2010 by Michal Nazarewicz    <mina86@mina86.com>
 *               and Maciej Swietochowski <m@swietochowski.eu>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef H_GLCONFIG_HPP
#define H_GLCONFIG_HPP

#include "../ui/abstract-config.hpp"

#include "quadric.hpp"

#define MAX_DISTANCE 1500.0

namespace gl {

namespace cfg {

struct Data;

}

typedef ui::Config<cfg::Data> Config;

namespace cfg {

struct Data : public ui::cfg::Data {
	ui::cfg::Real mouseMovementFactor, mouseRotationFactor;
	ui::cfg::Real runFactor, creepFactor;
	ui::cfg::Real cutOffDistance2;
	ui::cfg::Bool showText, showTextures, lowQuality;
	ui::cfg::Bool showStars;
	ui::cfg::List drawStyle;
	gl::Quadric quad;

	virtual struct iterator items() const;

private slots:
	void drawStyleChanged(long v);

private:
	Data();

	friend struct ui::Config<cfg::Data>;

	Q_OBJECT
};

}

}

#endif
