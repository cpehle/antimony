# coding=utf-8

from PySide import QtCore, QtGui

import base
import colors

class UnionControl(base.TextLabelControl):
    @classmethod
    def new(cls, canvas, x, y, z, scale):
        u = Union(get_name('u'), x, y, z)
        cls(canvas, u)

    def __init__(self, canvas, target):
        super(UnionControl, self).__init__(
                canvas, target, u"A ∪ B")
        self.editor_datums = ['name','A','B','shape']

class IntersectionControl(base.TextLabelControl):
    @classmethod
    def new(cls, canvas, x, y, z, scale):
        i = Intersection(get_name('i'), x, y, z)
        cls(canvas, i)

    def __init__(self, canvas, target):
        super(IntersectionControl, self).__init__(
                canvas, target, u"A ∩ B")
        self.editor_datums = ['name','A','B','shape']


class CutoutControl(base.TextLabelControl):
    @classmethod
    def new(cls, canvas, x, y, z, scale):
        c = Cutout(get_name('c'), x, y, z)
        cls(canvas, c)

    def __init__(self, canvas, target):
        super(CutoutControl, self).__init__(
                canvas, target, u"A ∩ ~B")
        self.editor_datums = ['name','A','B','shape']

from node.base import get_name
from node.csg import Union, Intersection, Cutout