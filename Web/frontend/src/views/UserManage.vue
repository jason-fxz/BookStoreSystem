<script setup>
import { ref, onMounted, reactive } from 'vue';
import axios from 'axios';

const selectButtonValue1 = ref(null);
const selectButtonValues1 = ref([{ name: 'Customer' }, { name: 'Admin' }]);


const message = ref([]);

const userdata = reactive({
    data: {
        username: '',
        nickname: '',
        password: '',
        level: 0,
    }
})
const count = ref(0)



const adduser = () => {
    if (selectButtonValue1.value == null) userdata.data.level = 0
    else userdata.data.level = selectButtonValue1.value.name == 'Customer' ? 1 : (selectButtonValue1.value.name == 'Admin' ? 3 : 0)
    message.value = []
    var flag = 0
    if (userdata.data.username == '') {
        message.value.push({ severity: 'error', detail: 'Error Message', content: 'Empty Username', id: count.value++ });
        flag = 1
    }
    if (userdata.data.nickname == '') {
        message.value.push({ severity: 'error', detail: 'Error Message', content: 'Empty Nickname', id: count.value++ });
        flag = 1
    }
    if (userdata.data.password == '') {
        message.value.push({ severity: 'error', detail: 'Error Message', content: 'Empty Password', id: count.value++ });
        flag = 1
    }
    if (userdata.data.level == 0) {
        message.value.push({ severity: 'error', detail: 'Error Message', content: 'Empty Level', id: count.value++ });
        flag = 1
    }
    if (flag == 1) {
        return false
    }
    axios.post('/api/adduser', JSON.stringify(userdata.data))
        .then(res => {
            console.log(res)
            if (res.status == 200) {
                message.value = [{ severity: 'info', detail: 'Info Message', content: `User ${JSON.stringify(userdata)}`, id: count.value++ },
                { severity: 'success', detail: 'Success Message', content: `Add User Success`, id: count.value++ }];
                book_isbn.value = bookdata.data.ISBN
                select()
            }
        })
        .catch(err => {
            console.log(err)
            if (err.response.status == 403) {
                console.log(err.response.data.error)
                message.value = [{ severity: 'error', detail: 'Error Message', content: 'Add User Failed: ' + err.response.data.error, id: count.value++ }];
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


</script>

<template>
    <div class="grid">
        <div class="col-12">
            <div class="card">
                <h2>User Manage</h2>

                <h3>Add User</h3>
                <div class="p-fluid formgrid grid">
                    <div class="field col-12 md:col-6">
                        <label for="UserName">Username</label>
                        <InputText id="UserName" v-model="userdata.data.username" type="text" />
                    </div>
                    <div class="field col-12 md:col-6">
                        <label for="NickName">Nickname</label>
                        <InputText id="NickName" v-model="userdata.data.nickname" type="text" />
                    </div>
                    <div class="field col-12 md:col-6">
                        <label for="Password">Password</label>
                        <Password id="Password" v-model="userdata.data.password" :feedback="false" toggleMask type="text"/>
                    </div>
                    <div class="field col-12 md:col-6">
                        <label for="Level">Level</label>
                        <SelectButton id="Level" v-model="selectButtonValue1" :options="selectButtonValues1" optionLabel="name" />
                    </div>

                    <div class="field col-12 md:col-6">
                        <label for="Adduser">click to adduser</label>
                        <Button id="Adduser" @click="adduser" label="Add User" class="mr-2 mb-2"></Button>
                    </div>

                </div>
                <transition-group name="p-message" tag="div">
                    <Message v-for="msg of message" :severity="msg.severity" :key="msg.content">{{ msg.content }}</Message>
                </transition-group>
               


               
            </div>
        </div>




    </div>
</template>

<style>
#keywordsinput {
    margin-top: 20px;
}
</style>