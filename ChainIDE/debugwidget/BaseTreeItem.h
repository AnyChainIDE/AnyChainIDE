#ifndef BaseTreeItem_h__
#define BaseTreeItem_h__

#include <QtCore/QVariant>
//////////////////////////////////////////////////////////////////////////
///<summary> 通用树结构类 </summary>
///
///<remarks> 朱正天,2017/11/1. </remarks>
///////////////////////////////////////////////////////////////////////////*/
class BaseTreeItem
{
public:
	BaseTreeItem(QVariant data = QVariant(), BaseTreeItem *parent = 0);
	virtual ~BaseTreeItem(void);
public:
	//////////////////////////////////////////////////////////////////////////
	///<summary>  添加直属下级.</summary>
	///
	///<remarks> 朱正天,2017/1/8.</remarks>
	///////////////////////////////////////////////////////////////////////////
	void appendChild(BaseTreeItem *child);

	//////////////////////////////////////////////////////////////////////////
	///<summary>  通过行号，获取下级指针.</summary>
	///
	///<remarks> 朱正天,2017/1/8.</remarks>
	///////////////////////////////////////////////////////////////////////////
	BaseTreeItem *GetChild(int row);

	//////////////////////////////////////////////////////////////////////////
	///<summary>  下级数量.</summary>
	///
	///<remarks> 朱正天,2017/1/8.</remarks>
	///////////////////////////////////////////////////////////////////////////
	int childCount() const;

	//////////////////////////////////////////////////////////////////////////
	///<summary>  获取数据.</summary>
	///
	///<remarks> 朱正天,2017/1/8.</remarks>
	///////////////////////////////////////////////////////////////////////////
	QVariant data() const;

    //////////////////////////////////////////////////////////////////////////
    ///<summary>  数据.</summary>
    ///
    ///<remarks> 朱正天,2017/1/8.</remarks>
    ///////////////////////////////////////////////////////////////////////////
    void setData(const QVariant &var);

	//////////////////////////////////////////////////////////////////////////
	///<summary>  获取自身在父类中的行号.</summary>
	///
	///<remarks> 朱正天,2017/1/8.</remarks>
	///////////////////////////////////////////////////////////////////////////
	int RowInParent() const;

	//////////////////////////////////////////////////////////////////////////
	///<summary>  获取父亲.</summary>
	///
	///<remarks> 朱正天,2017/1/8.</remarks>
	///////////////////////////////////////////////////////////////////////////
	BaseTreeItem *GetParent();

	//////////////////////////////////////////////////////////////////////////
	///<summary>  设置父亲.</summary>
	///
	///<remarks> 朱正天,2017/1/8.</remarks>
	///////////////////////////////////////////////////////////////////////////
	void SetParent(BaseTreeItem *parentItem);

	//////////////////////////////////////////////////////////////////////////
	///<summary>  删除下级.</summary>
	///
	///<remarks> 朱正天,2017/1/9.</remarks>
	///////////////////////////////////////////////////////////////////////////
	bool removeChild(BaseTreeItem *childItem);
public:
	///<summary> 勾选属性 --朱正天,2017.12.9</summary>///
	void SetCheckState(Qt::CheckState checkState, bool childEffect, bool parentEffect);
	Qt::CheckState GetCheckState()const;
private:
	void updateChildCheckState(Qt::CheckState checkState);
	void updateParentCheckState(Qt::CheckState checkState);
private:
	class BaseTreeItemPrivate;
	BaseTreeItemPrivate *_p;

};

#endif // BaseTreeItem_h__