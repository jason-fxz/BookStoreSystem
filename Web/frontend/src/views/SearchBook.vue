<script setup>
import { FilterMatchMode, FilterOperator } from 'primevue/api';
import { ref, onBeforeMount, onMounted } from 'vue';
import axios from 'axios';

const booklist = ref([]);
// const customer2 = ref(null);
// const customer3 = ref(null);
const filters1 = ref(null);
const loading1 = ref(null);
// const loading2 = ref(null);
// const products = ref(null);
// const expandedRows = ref([]);



onBeforeMount(() => {
    initFilters1();
});

function getAllBooks() {
    axios.post('/api/searchbook', JSON.stringify({
        searchtype: "All",
        searchtext: ""
    }))
        .then(res => {
            console.log(res)
            if (res.status == 200) {
                booklist.value = res.data.data;
            }
        })
        .catch(err => {
            console.log(err)
            if (err.response.status == 403) {
                console.log(err.response.data.error)
                showError("Error", err.response.data.error);
            }
        })
}

onMounted(() => {
    getAllBooks();
    initFilters1();
});

const initFilters1 = () => {
    filters1.value = {
        global: { value: null, matchMode: FilterMatchMode.CONTAINS },
        ISBN: { operator: FilterOperator.AND, constraints: [{ value: null, matchMode: FilterMatchMode.EQUALS }] },
        name: { operator: FilterOperator.AND, constraints: [{ value: null, matchMode: FilterMatchMode.STARTS_WITH }] },
        author: { operator: FilterOperator.AND, constraints: [{ value: null, matchMode: FilterMatchMode.CONTAINS }] },
        keywords: { operator: FilterOperator.AND, constraints: [{ value: null, matchMode: FilterMatchMode.CONTAINS }] },
        // representative: { value: null, matchMode: FilterMatchMode.IN },
        // date: { operator: FilterOperator.AND, constraints: [{ value: null, matchMode: FilterMatchMode.DATE_IS }] },
        // balance: { operator: FilterOperator.AND, constraints: [{ value: null, matchMode: FilterMatchMode.EQUALS }] },
        // status: { operator: FilterOperator.OR, constraints: [{ value: null, matchMode: FilterMatchMode.EQUALS }] },
        // activity: { value: [0, 50], matchMode: FilterMatchMode.BETWEEN },
        // verified: { value: null, matchMode: FilterMatchMode.EQUALS }
    };
};

const clearFilter1 = () => {
    initFilters1();
};
</script>

<template>
    <div class="grid">
        <div class="col-12">
            <div class="card">
                <h2>Search</h2>
                <DataTable :value="booklist" :paginator="true" class="p-datatable-gridlines" :rows="10" dataKey="id"
                    :rowHover="true" v-model:filters="filters1" filterDisplay="menu" :loading="loading1" :filters="filters1"
                    responsiveLayout="scroll" :globalFilterFields="['ISBN', 'name', 'author', 'keywords']">
                    <template #header>
                        <div class="flex justify-content-between flex-column sm:flex-row">
                            <Button type="button" icon="pi pi-filter-slash" label="Clear" class="p-button-outlined mb-2"
                                @click="clearFilter1()" />
                            <span class="p-input-icon-left mb-2">
                                <i class="pi pi-search" />
                                <InputText v-model="filters1['global'].value" placeholder="Search" style="width: 100%" />
                            </span>
                        </div>
                    </template>
                    <template #empty> No Books found. </template>
                    <template #loading> Loading Books data. Please wait. </template>

                    <Column field="ISBN" header="ISBN" style="min-width: 12rem">
                        <template #body="{ data }">
                            {{ data.ISBN }}
                        </template>
                        <template #filter="{ filterModel }">
                            <InputText type="text" v-model="filterModel.value" class="p-column-filter"
                                placeholder="Search by ISBN" />
                        </template>
                    </Column>

                    <Column field="name" header="BookName" style="min-width: 12rem">
                        <template #body="{ data }">
                            {{ data.name }}
                        </template>
                        <template #filter="{ filterModel }">
                            <InputText type="text" v-model="filterModel.value" class="p-column-filter"
                                placeholder="Search by name" />
                        </template>
                    </Column>

                    <Column field="author" header="Author" style="min-width: 12rem">
                        <template #body="{ data }">
                            {{ data.author }}
                        </template>
                        <template #filter="{ filterModel }">
                            <InputText type="text" v-model="filterModel.value" class="p-column-filter"
                                placeholder="Search by author" />
                        </template>
                    </Column>

                    <Column field="keywords" header="Keywords" style="min-width: 12rem">
                        <template #body="{ data }">
                            <Chip v-for="(keyword, index) in data.keywords" :key="index" :label="keyword" class="mr-2 mb-2">
                            </Chip>
                        </template>
                        <template #filter="{ filterModel }">
                            <InputText type="text" v-model="filterModel.value" class="p-column-filter"
                                placeholder="Search by author" />
                        </template>
                    </Column>

                    <Column field="price" header="Price" style="min-width: 12rem">
                        <template #body="{ data }">
                            {{ "¥" + data.price }}
                        </template>
                    </Column>

                </DataTable>
            </div>
        </div>

    </div>
</template>

<style scoped lang="scss">
::v-deep(.p-datatable-frozen-tbody) {
    font-weight: bold;
}

::v-deep(.p-datatable-scrollable .p-frozen-column) {
    font-weight: bold;
}
</style>
