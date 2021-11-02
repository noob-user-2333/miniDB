//
// Created by user on 2021/10/30.
//

#ifndef MINIDB_OVERFLOWPAGE_H
#define MINIDB_OVERFLOWPAGE_H



class OverflowPage {
private:


    VOID FreePageIDCount(UINT free_page_id_count);
public:
    DiskPage *page;
    const INT HEADER_SIZE = sizeof(UINT) * 2;
    OverflowPage(DiskPage *page) {
        if (page)
            this->page = page;
        else
        {
            std::cout << "生成OverFlowPage对应DiskPage不可为空" << std::endl;
            _exit(0);
        }
    }

    UINT PageID()
    {
        return page->PageID();
    }
    VOID NextPageID(UINT next_page_id);

    UINT NextPageID();

    UINT FreePageIDCount();

    VOID Format()
    {
        page->Format();
    }


    INT PageIDPush(UINT page_id)
    {
        if(page_id < 2)
            return DB_INVALID_PARA;
        UINT count = FreePageIDCount();
        if(count * sizeof(UINT) + HEADER_SIZE >= page->Size())
            return DB_NO_FREE_SPACE;
        INT status = page->Write(&page_id,sizeof(page_id),count * sizeof(UINT) + HEADER_SIZE);
        if(status != DB_SUCCESS)
            return status;
        FreePageIDCount(count+1);
        return DB_SUCCESS;
    }

    INT PageIDPop()
    {
        if(FreePageIDCount() == 0)
            return DB_IS_NULL;
        FreePageIDCount(FreePageIDCount()-1);
        return DB_SUCCESS;
    }

    INT TopOfPageID(UINT *dest_ptr)
    {
        if(FreePageIDCount() == 0)
            return DB_IS_NULL;
        return page->Read(dest_ptr,sizeof(UINT),FreePageIDCount() * sizeof(UINT) + HEADER_SIZE - sizeof(UINT));
    }


};


UINT OverflowPage::NextPageID() {
    UINT result;
    page->Read(&result, sizeof(result), 0);
    return result;
}

VOID OverflowPage::NextPageID(UINT next_page_id) {
    page->Write(&next_page_id, sizeof(next_page_id), 0);
}

UINT OverflowPage::FreePageIDCount() {
    UINT result;
    page->Read(&result, sizeof(result), 4);
    return result;
}

VOID OverflowPage::FreePageIDCount(UINT free_page_id_count) {
    page->Write(&free_page_id_count, sizeof(free_page_id_count), 4);
}

#endif //MINIDB_OVERFLOWPAGE_H
