<script setup>
import { ref, onMounted } from 'vue';
import axios from 'axios';

import { useToast } from 'primevue/usetoast';
const toast = useToast();

const loginfo = ref('')
const workerinfo = ref('')
const tradeinfo = ref('')
const loading1 = ref(true)
const loading2 = ref(true)
const loading3 = ref(true)

const showError = (title, message) => {
    toast.add({ severity: 'error', summary: title, detail: message, life: 3000 });
};

const Getlogs = () => {
    axios.post('/api/getlog', JSON.stringify({
        logtype: "log",
    }))
        .then(res => {
            console.log(res)
            if (res.status == 200) {
                loading1.value = false
                loginfo.value = res.data
            }
        })
        .catch(err => {
            console.log(err)
            if (err.response.status == 403) {
                showError("Error", err.response.data.error);
            }
        })

    axios.post('/api/getlog', JSON.stringify({
        logtype: "work",
    }))
        .then(res => {
            console.log(res)
            if (res.status == 200) {
                loading2.value = false
                workerinfo.value = res.data
            }
        })
        .catch(err => {
            console.log(err)
            if (err.response.status == 403) {
                showError("Error", err.response.data.error);
            }
        })
    axios.post('/api/getlog', JSON.stringify({
        logtype: "trade",
    }))
        .then(res => {
            console.log(res)
            if (res.status == 200) {
                loading3.value = false
                tradeinfo.value = res.data
            }
        })
        .catch(err => {
            console.log(err)
            if (err.response.status == 403) {
                showError("Error", err.response.data.error);
            }
        })




    // addMessage('success');
};


onMounted(() => {
    console.log('ViewLogs.vue onMounted');
    setTimeout(() => {
        Getlogs()
    }, 500);

})

</script>

<template>
    <div class="grid">
        <div class="col-12">
            <div class="card">
                <h3>Logs INFO</h3>
                <Skeleton v-if="loading1" width="100%" height="150px" ></Skeleton>
                <ScrollPanel v-if="!loading1" :style="{ height: '40em' }">
                    <pre class="codestyle">{{ loginfo }}</pre>
                    <ScrollTop target="parent" :threshold="100" icon="pi pi-arrow-up"></ScrollTop>
                </ScrollPanel>
            </div>
        </div>

        <div class="col-12">
            <div class="card">
                <h3>Workers INFO</h3>
                <Skeleton v-if="loading2" width="100%" height="150px"></Skeleton>
                <ScrollPanel v-if="!loading2" :style="{ height: '40em' }">
                    <pre class="codestyle">{{ workerinfo }}</pre>
                    <ScrollTop target="parent" :threshold="100" icon="pi pi-arrow-up"></ScrollTop>
                </ScrollPanel>
            </div>
        </div>

        <div class="col-12">
            <div class="card">
                <h3>finance INFO</h3>
                <Skeleton v-if="loading3" width="100%" height="150px"></Skeleton>
                <ScrollPanel v-if="!loading3" :style="{ height: '40em' }">
                <pre class="codestyle">{{ tradeinfo }}</pre>
                <ScrollTop target="parent" :threshold="100" icon="pi pi-arrow-up"></ScrollTop>
                </ScrollPanel>

            </div>
        </div>




    </div>
</template>

<style>
.codestyle {
    font-family: Consolas, Monaco, 'Andale Mono', 'Ubuntu Mono', monospace;
    font-size: 12px;
    line-height: 1.5;
    color: #333;
    background-color: #f5f5f5;
    border-radius: 4px;
    padding: 10px;
    margin: 10px 0;
    overflow: auto;
    line-height: 1.5;
}
</style>