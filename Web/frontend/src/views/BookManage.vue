<script setup>
import { ref, onMounted, reactive } from 'vue';
import axios from 'axios';



const book_isbn = ref('');
const book_count = ref(1);
const tempkeyword = ref('')
const book_import_count = ref(1)
const book_import_cost = ref(0.00)
const bookdata = reactive({
    data: {
        ISBN: '',
        name: '',
        author: '',
        keywords: [],
        price: 0,
        quantity: 0,
    }
})
const count = ref(0)

const AddKeyword = () => {
    if (tempkeyword.value == '') {
        message.value = [{ severity: 'error', detail: 'Error Message', content: 'You cannot add empty keyword', id: count.value++ }];
        return false
    }
    bookdata.data.keywords.push(tempkeyword.value)
    tempkeyword.value = ''
}

const erasekey = (index) => {
    bookdata.data.keywords.splice(index, 1)
}

const select = () => {
    if (book_isbn.value == '') {
        message.value = [{ severity: 'error', detail: 'Error Message', content: 'ISBN is empty', id: count.value++ }];
        return false
    }
    console.log(book_isbn.value);
    console.log(book_count.value);
    axios.post('/api/selectbook', JSON.stringify({
        ISBN: book_isbn.value,
    }))
        .then(res => {
            console.log(res)
            if (res.status == 200) {
                bookdata.data = res.data.data
            }
        })
        .catch(err => {
            console.log(err)
            if (err.response.status == 403) {
                console.log(err.response.data.error)
                message.value = [{ severity: 'error', detail: 'Error Message', content: 'Book Select Failed: ' + err.response.data.error, id: count.value++ }];
            }
        })
};

const modify = () => {
    if (book_isbn.value == '') {
        message.value = [{ severity: 'error', detail: 'Error Message', content: 'No selected book', id: count.value++ }];
        return false
    }
    axios.post('/api/modifybook', JSON.stringify({
        oldISBN: book_isbn.value,
        ISBN: bookdata.data.ISBN,
        name: bookdata.data.name,
        author: bookdata.data.author,
        keywords: bookdata.data.keywords,
        price: bookdata.data.price,
        quantity: bookdata.data.quantity,
    }))
        .then(res => {
            console.log(res)
            if (res.status == 200) {
                message.value = [{ severity: 'info', detail: 'Info Message', content: `Book ${book_isbn.value}: ${JSON.stringify(bookdata)}`, id: count.value++ },
                { severity: 'success', detail: 'Success Message', content: `Book Modify Success`, id: count.value++ }];
                book_isbn.value = bookdata.data.ISBN
                select()
            }
        })
        .catch(err => {
            console.log(err)
            if (err.response.status == 403) {
                console.log(err.response.data.error)
                message.value = [{ severity: 'error', detail: 'Error Message', content: 'Book Modify Failed: ' + err.response.data.error, id: count.value++ }];
            }
        })
}

const importbook = () => {
    if (book_isbn.value == '') {
        message.value = [{ severity: 'error', detail: 'Error Message', content: 'No selected book', id: count.value++ }];
        return false
    }
    axios.post('/api/importbook', JSON.stringify({
        ISBN: book_isbn.value,
        quantity: book_import_count.value,
        totalcost: book_import_cost.value.toString(),
    }))
        .then(res => {
            console.log(res)
            if (res.status == 200) {
                message.value = [{ severity: 'info', detail: 'Info Message', content: `Book ${book_isbn.value}: import ${book_import_count.value} with cost of ${book_import_cost.value}`, id: count.value++ },
                { severity: 'success', detail: 'Success Message', content: `Book Import Success`, id: count.value++ }];
            }
            select()
        })
        .catch(err => {
            console.log(err)
            if (err.response.status == 403) {
                console.log(err.response.data.error)
                message.value = [{ severity: 'error', detail: 'Error Message', content: 'Book Import Failed: ' + err.response.data.error, id: count.value++ }];
            }
        })

}

const addMessage = (type) => {
    if (type === 'success') {
        message.value = [{ severity: 'success', detail: 'Success Message', content: 'Book Purchase Successfully', id: count.value++ }];
    } else if (type === 'info') {
        message.value = [{ severity: 'info', detail: 'Info Message', content: 'PrimeVue rocks', id: count.value++ }];
    } else if (type === 'warn') {
        message.value = [{ severity: 'warn', detail: 'Warn Message', content: 'There are unsaved changes', id: count.value++ }];
    } else if (type === 'error') {
        message.value = [{ severity: 'error', detail: 'Error Message', content: 'Validation failed', id: count.value++ }];
    }
};

const message = ref([]);

</script>

<template>
    <div class="grid">
        <div class="col-12">
            <div class="card">
                <h2>Book Manage</h2>
                <div class="p-fluid formgrid grid">
                    <div class="field col-12 md:col-6">
                        <label for="ISBN">ISBN</label>
                        <InputText id="ISBN" v-model="book_isbn" type="text" />
                    </div>
                    <div class="field col-12 md:col-6">
                        <label for="select">click to selcet</label>
                        <Button id="select" @click="select" label="select" class="mr-2 mb-2"></Button>
                    </div>

                </div>
                <transition-group name="p-message" tag="div">
                    <Message v-for="msg of message" :severity="msg.severity" :key="msg.content">{{ msg.content }}</Message>
                </transition-group>
                <h2>Modify</h2>
                <div class="p-fluid formgrid grid">
                    <div class="field col-12 md:col-6">
                        <label for="BookISBN">ISBN</label>
                        <InputText id="BookISBN" v-model="bookdata.data.ISBN" type="text" />
                    </div>
                    <div class="field col-12 md:col-6">
                        <label for="BookName">Name</label>
                        <InputText id="BookName" v-model="bookdata.data.name" type="text" />
                    </div>
                    <div class="field col-12 md:col-6">
                        <label for="BookAuthor">Author</label>
                        <InputText id="BookAuthor" v-model="bookdata.data.author" type="text" />
                    </div>
                    <div class="field col-12 md:col-6">
                        <label for="BookPrice">Price</label>
                        <InputNumber id="BookPrice" v-model="bookdata.data.price" mode="currency" currency="RMB"/>
                    </div>

                    <div class="field col-12 md:col-6">
                        <label for="KeyWords">KeyWords</label>
                        <div class="p-inputgroup">
                            <InputText id="KeyWords" v-model="tempkeyword" />
                            <Button label="Add Keyword" @click="AddKeyword()" />
                        </div>

                        <div id="keywordsinput">
                            <Chip v-for="(keyword, index) in bookdata.data.keywords" :key="index" :label="keyword"
                                class="mr-2 mb-2" @click="erasekey(index)">
                            </Chip>
                        </div>
                    </div>

                    <div class="field col-12 md:col-6">
                        <label for="modify">click to modify</label>
                        <Button id="modify" @click="modify" label="modify" class="mr-2 mb-2"></Button>
                    </div>

                </div>



                <h2>Import</h2>
                <div class="p-fluid formgrid grid">
                    <div class="field col-12 md:col-6">
                        <label for="BookQuantity">Quantity</label>
                        <InputNumber disabled id="BookQuantity" v-model="bookdata.data.quantity"  mode="decimal" :min="0" :max="2147483648"/>
                    </div>
                    <div class="field col-12 md:col-6">
                        <label for="BookCount">Count</label>
                        <InputNumber id="BookCount" v-model="book_import_count"  mode="decimal" :min="1" :max="2147483648"/>
                    </div>

                    <div class="field col-12 md:col-6">
                        <label for="totalCost">totalCost</label>
                        <InputNumber id="totalCost" v-model="book_import_cost"  mode="currency" currency="RMB" />
                    </div>

                    <div class="field col-12 md:col-6">
                        <label for="importbook">click to import</label>
                        <Button id="importbook" @click="importbook" label="import" class="mr-2 mb-2"></Button>
                    </div>

                </div>
            </div>
        </div>




    </div>
</template>

<style>
#keywordsinput {
    margin-top: 20px;
}
</style>