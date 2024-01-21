<script setup>
import { ref, onMounted } from 'vue';
import axios from 'axios';



const book_isbn = ref('');
const book_count = ref(1);
const count = ref(0)




const purchase = () => {
    if (book_isbn.value == '') {
        message.value = [{ severity: 'error', detail: 'Error Message', content: 'ISBN is empty', id: count.value++ }];
        return false
    }
    if (book_count.value == 0) {
        message.value = [{ severity: 'error', detail: 'Error Message', content: 'Count is 0', id: count.value++ }];
        return false
    }
    console.log(book_isbn.value);
    console.log(book_count.value);
    axios.post('/api/purchasebook', JSON.stringify({
        ISBN: book_isbn.value,
        quantity: book_count.value
    }))
        .then(res => {
            console.log(res)
            if (res.status == 200) {
                message.value = [{ severity: 'info', detail: 'Info Message', content: `ISBN: ${ISBN.value} Count: ${book_count.value}  totalCost: ${res.data.data.totalcost}`, id: count.value++ },
                { severity: 'success', detail: 'Success Message', content: 'Book Purchase Successfully', id: count.value++ }]
            }
        })
        .catch(err => {
            console.log(err)
            if (err.response.status == 403) {
                console.log(err.response.data.error)
                message.value = [{ severity: 'error', detail: 'Error Message', content: 'Book Purchase Failed: ' + err.response.data.error, id: count.value++ }];
            }
        })




    // addMessage('success');
};

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
                <h2>Purchase</h2>
                <div class="p-fluid formgrid grid">
                    <div class="field col-12 md:col-6">
                        <label for="ISBN">ISBN</label>
                        <InputText id="ISBN" v-model="book_isbn" type="text" />
                    </div>
                    <div class="field col-12 md:col-6">
                        <label for="count">Count</label>
                        <InputNumber id="count" v-model="book_count" inputId="minmax-buttons" mode="decimal" showButtons :min="0" :max="2147483648" />
                    </div>
                    <div class="field col-12">
                        <Button @click="purchase" label="purchase" class="mr-2 mb-2"></Button>
                    </div>

                </div>
                <transition-group name="p-message" tag="div">
                    <Message v-for="msg of message" :severity="msg.severity" :key="msg.content">{{ msg.content }}</Message>
                </transition-group>
            </div>
        </div>




    </div>
</template>
