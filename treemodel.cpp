/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

/*
    treemodel.cpp

    Provides a simple tree model to show how to create and use hierarchical
    models.
*/

#include "treemodel.h"
#include "treeitem.h"
#include <QDebug>

#include <QStringList>

//! [0]
TreeModel::TreeModel(QHash<QString, QStringList> data, QObject *parent)
    : QAbstractItemModel(parent)
{
    rootItem = new TreeItem({tr("ДН"), tr("№SAP")});
    setupModelData(data, rootItem);
}
//! [0]c

//! [1]
TreeModel::~TreeModel()
{
    delete rootItem;
}
//! [1]

//! [2]
int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    return rootItem->columnCount();
}
//! [2]

//! [3]
QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return item->data(index.column());
}
//! [3]

//! [4]
Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}
//! [4]

//! [5]
QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}
//! [5]

//! [6]
QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}
//! [6]

//! [7]
QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}
//! [7]

//! [8]
int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}
//! [8]

void TreeModel::setupModelData(QHash<QString, QStringList> data, TreeItem *parent)
{
    QRegExp exPS( "PS[0-9][0-9][0-9]-[0-9][0-9][0-9][0-9][0-9][0-9]");
    QRegExp exTP( "TP[0-9][0-9][0-9]-[0-9][0-9][0-9][0-9][0-9][0-9][0-9]");
    QRegExp exVS("VS[0-9][0-9][0-9]-[0-9][0-9][0-9][0-9][0-9][0-9][0-9]");
    QRegExp exVS001("VS[0-9][0-9][0-9]-[0-9][0-9][0-9][0-9][0-9][0-9][0-9]-[0,3]01");
    QRegExp exVN( "VN[0-9][0-9][0-9]-[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]");
    QRegExp exVN001( "VN[0-9][0-9][0-9]-[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]-[0,2,3]01");


    foreach (QStringList value, data){
        QVector<QVariant> columnData;
        columnData.reserve(4);
        for (const QString &columnString : value)  columnData << columnString;
        TreeItemS.insert(value[0],new TreeItem(columnData));
    }

    foreach (QStringList value, data){
        if(exPS.exactMatch(value[0])) {
            TreeItem* itt = (TreeItemS[value[0]]);
            itt->setParentItem(parent);
            parent->appendChild(itt);
        }

        if (value[2]!="" && TreeItemS.contains(value[2]) && !exVS001.exactMatch(value[0])  && !exVN001.exactMatch(value[0]))
        {
            TreeItem* itt = (TreeItemS[value[0]]);
            itt->setParentItem(TreeItemS[value[2]]);
            TreeItemS[value[2]]->appendChild(itt);
        }
    }

    foreach (TreeItem* TItem, TreeItemS)
    {


        QString sapNumber = TItem->data(0).toString();

        if(QRegExp("[V,T,P].*").exactMatch(sapNumber) && TItem->parentItem()==nullptr)
        {
            QString parrentNumber = sapNumber.left(sapNumber.lastIndexOf("-"));

            if (TreeItemS.contains(parrentNumber))
            {
                TreeItem* parrentIt = TreeItemS[parrentNumber];
                TItem->setParentItem(parrentIt);
                parrentIt->appendChild(TItem);
            }


        }


    }

    foreach (TreeItem* TItem, TreeItemS)
    {
        QString sapNumber = TItem->data(0).toString();
        QString itemName = TItem->data(1).toString();
        if(exVS.exactMatch(sapNumber))
        {
            // PS035-000458-14-02-001-10
            QString fidName  = itemName.left(16).right(7);
            if (TItem->parentItem()==nullptr) continue;
            QString parrentNum = TItem->parentItem()->data(0).toString();

            foreach (TreeItem* TItemm, TreeItemS)
            {
                QString sapNumberr = TItemm->data(0).toString();
                QString itemmName = TItemm->data(1).toString();
                QString reggStr = parrentNum + "-14-0[0-9]-[0-9][0-9][0-9]-[0-9][0-9]";

                if(QRegExp(reggStr).exactMatch(sapNumberr))
                {
                    if (itemmName.contains(fidName))
                    {

                        TItem->parentItem()->removeChild(TItem);
                        TItem->setParentItem(TItemm);
                        TItemm->appendChild(TItem);
                        qDebug()<<sapNumberr<<"  "<<itemmName<<endl;
                    }

                }
            }

        }



        if(exTP.exactMatch(sapNumber))
        {
             //VS010-0006181-503-4501     Спуск: от оп.34 до ТП-1839
             //TP010-0025065              ТП 32-05-1839

            if (TItem->parentItem()==nullptr) continue;
            QString tpName  = itemName.right(itemName.count()-itemName.lastIndexOf("-")-1);
            QString lineNum =  TItem->parentItem()->data(0).toString();

            tpName.remove(QRegExp("[^A-Za-z0-9]"));

            qDebug()<<sapNumber<<" "<< tpName<<endl;
            foreach (TreeItem* TItemm, TreeItemS)
            {
                QString sapNumberr = TItemm->data(0).toString();
                QString itemmName = TItemm->data(1).toString();
                QString reggStr = lineNum + ".*45[0-9][0-9]";

                if(QRegExp(reggStr).exactMatch(sapNumberr))
                {
                    if (itemmName.contains(tpName))
                    {

                        TItem->parentItem()->removeChild(TItem);
                        TItem->setParentItem(TItemm);
                        TItemm->appendChild(TItem);
                       // qDebug()<<sapNumberr<<"  "<<itemmName<<endl;
                    }

                }
            }


        }
    }


}

